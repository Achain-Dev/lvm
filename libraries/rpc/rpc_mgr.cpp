#include <client/client.hpp>
#include <rpc/rpc_mgr.hpp>
#include <rpc/stcp_socket.hpp>
#include <rpc/rpc_msg.hpp>
#include <base/exceptions.hpp>
#include <iostream>

const uint32_t MAX_MESSAGE_SIZE = (512 * 10000 * 5);
const int BUFFER_SIZE = 16;


RpcMgr::RpcMgr(Client* client)
    :_socket_thread_ptr(std::make_shared<fc::thread>("sync_sock_server")),
     _hello_thread_ptr(std::make_shared<fc::thread>("hello_msg_send")),
     _b_valid_flag(false),
     _b_send_hello(false),
     _rpc_handler_ptr(std::make_shared<RpcTaskHandler>(this)) {
    _client_ptr = client;
}

RpcMgr::~RpcMgr() {
    close_connection();
    _rpc_server.close();
    _socket_thread_ptr->quit();
}

void RpcMgr::start() {
    if (!_b_valid_flag) {
        FC_THROW_EXCEPTION(lvm::global_exception::rpc_exception, \
                           "rpc server configuration error, please set the endpoint. ");
    }
    
    _rpc_server.set_reuse_address();
    _rpc_server.listen(_end_point);
    _socket_thread_ptr->async([&]() {
        accept_loop();
    });
    _terminate_hello_loop_done = _hello_thread_ptr->async([&]() {
        send_hello_msg_loop();
    });
}

void RpcMgr::accept_loop() {
    if (!_rpc_handler_ptr) {
        FC_THROW_EXCEPTION(lvm::global_exception::rpc_pointrt_null, \
                           "rpc process is null, please set the rpc processor. ");
    }
    
    while (true) {
        try {
            _rpc_server.accept(_rpc_connection.get_socket());
            _rpc_connection.accept();
            process_rpc();
            
        } catch (fc::exception& e) {
            close_connection();
            elog("close connection ${e}", ("e", e.to_detail_string()));
        }
    }
}

void RpcMgr::process_rpc() {
    _b_send_hello = true;
    fc::async([&]() {
        read_loop();
    }).wait();
    return;
}


void RpcMgr::set_endpoint(std::string& ip_addr, int port) {
    fc::ip::address ip(ip_addr);
    _end_point = fc::ip::endpoint(ip, port);
    _b_valid_flag = true;
    return;
}

void RpcMgr::close_connection() {
    _rpc_connection.close();
    _b_send_hello = false;
    return;
}

uint32_t RpcMgr::read_message(std::string& msg_str) {
    uint64_t bytes_received = 0;
    uint64_t remaining_bytes_with_padding = 0;
    char* buffer_sock = NULL;
    MessageHeader m;
    char buffer[BUFFER_SIZE];
    int leftover = BUFFER_SIZE - sizeof(MessageHeader);
    /*first: read msgHead, get data.size*/
    _rpc_connection.read(buffer, BUFFER_SIZE);
    /*convert to MessageHeader*/
    memcpy((char*)&m, buffer, sizeof(MessageHeader));
    FC_ASSERT(m.size <= MAX_MESSAGE_SIZE, "", ("m.size", m.size)("MAX_MESSAGE_SIZE", MAX_MESSAGE_SIZE));
    /*the total len of the msg:(header + data)*/
    bytes_received = 16 * ((sizeof(MessageHeader) + m.size + 15) / 16);
    buffer_sock = new char[bytes_received];
    memset(buffer_sock, 0, bytes_received);
    memcpy(buffer_sock, buffer, BUFFER_SIZE);
    /*remaining len of byte to read from socket*/
    remaining_bytes_with_padding = 16 * ((m.size - leftover + 15) / 16);
    
    /*read the remain bytes*/
    try {
        if (remaining_bytes_with_padding) {
            _rpc_connection.read(buffer_sock + BUFFER_SIZE, remaining_bytes_with_padding);
        }
        
    } catch (...) {
        delete[] buffer_sock;
        FC_THROW_EXCEPTION(lvm::global_exception::socket_read_error, \
                           "socket read error. ");
    }
    
    msg_str = std::string(buffer_sock, bytes_received);
    delete[] buffer_sock;
    return m.msg_type;
}

void RpcMgr::read_loop() {
    std::string msg_str = "";
    uint32_t type = 0;
    bool b_need_restart = false;
    
    try {
        while (true) {
            type = read_message(msg_str);
            
            if (type == LUA_REQUEST_RESULT_MESSAGE_TYPE) {
                _rpc_handler_ptr->set_value(msg_str);
                
            } else {
                _rpc_handler_ptr->handle_task(msg_str, nullptr);
            }
        }
        
    } catch (lvm::global_exception::socket_read_error& e) {
        wlog("disconnected ${e}", ("e", e.to_detail_string()));
        b_need_restart = true;
        
    } catch (...) {
        b_need_restart = true;
    }
    
    if (b_need_restart) {
        b_need_restart = false;
        close_connection();
        _terminate_hello_loop_done.cancel();
    }
}

void RpcMgr::send_to_chain(Message& m) {
    uint32_t size_of_message_and_header = 0;
    uint32_t size_with_padding = 0;
    size_of_message_and_header = sizeof(MessageHeader) + m.size;
    //pad the message we send to a multiple of 16 bytes
    size_with_padding = 16 * ((size_of_message_and_header + 15) / 16);
    std::unique_ptr<char[]> padded_message(new char[size_with_padding]);
    memcpy(padded_message.get(), (char*)&m, sizeof(MessageHeader));
    memcpy(padded_message.get() + sizeof(MessageHeader), m.data.data(), m.size);
    
    //send
    try {
        _rpc_connection.write(padded_message.get(), size_with_padding);
        _rpc_connection.flush();
        
    } catch (...) {
        elog("send message exception");
        FC_THROW_EXCEPTION(lvm::global_exception::socket_send_error, \
                           "socket send error. ");
    }
}
void RpcMgr::post_message(Message& rpc_msg) {
    //send response
    fc::sync_call(_socket_thread_ptr.get(), [&]() {
        try {
            send_to_chain(rpc_msg);

        } catch (lvm::global_exception::socket_send_error& e) {
            close_connection();
            FC_THROW_EXCEPTION(lvm::global_exception::async_socket_error, \
                               "async socket error: async send message error. ");
        };
    }, "post_message");
}

//send hello msg
void RpcMgr::send_hello_message() {
    uint32_t msg_len = 0;
    char* p_msg = nullptr;
    HelloMsgResultRpc hello_msg;
    
    if (_b_send_hello) {
        hello_msg.data.task_from = FROM_RPC;
        hello_msg.data.task_type = HELLO_MSG;
        Message msg(hello_msg);
        post_message(msg);
    }
}

void RpcMgr::send_hello_msg_loop() {
    send_hello_message();
    
    if (!_terminate_hello_loop_done.canceled()) {
        _terminate_hello_loop_done = fc::schedule([this]() {
            send_hello_msg_loop();
        },
        fc::time_point::now() + fc::seconds(SEND_HELLO_MSG_INTERVAL),
        "send_hello_msg_loop");
    }
}
