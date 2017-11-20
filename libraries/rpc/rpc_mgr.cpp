
#include <client/client.hpp>
#include <rpc/rpc_mgr.hpp>
#include <rpc/stcp_socket.hpp>
#include <rpc/rpc_msg.hpp>
#include <base/exceptions.hpp>
#include <iostream>

const uint32_t MAX_MESSAGE_SIZE = (512 * 10000 * 5);
const int BUFFER_SIZE = 16;


RpcMgr::RpcMgr(Client* client)
    :_sync_thread_ptr(std::make_shared<fc::thread>("sync_sock_server")),
     _async_thread_ptr(std::make_shared<fc::thread>("async_sock_server")),
     _b_valid_flag(false),
     _rpc_handler_ptr(std::make_shared<RpcTaskHandler>(this)) {
    _client_ptr = client;
}

RpcMgr::~RpcMgr() {
    close_connections();
    _rpc_server.close();
    _sync_thread_ptr->quit();
    _async_thread_ptr->quit();
}

void RpcMgr::start() {
    if (!_b_valid_flag) {
        FC_THROW_EXCEPTION(lvm::global_exception::rpc_exception, \
                           "rpc server configuration error, please set the endpoint. ");
    }
    
    _rpc_server.set_reuse_address();
    _rpc_server.listen(_end_point);
    fc::async([&]() {
        this->accept_loop();
    });
}

//in _rpc_connections, the first socket  is async,the second socket is sync
void RpcMgr::process_connection(SocketMode emode) {
    if (emode >= MODE_COUNT) {
        FC_THROW_EXCEPTION(lvm::global_exception::socket_mode_error, \
                           "rpc mode error. ");
    }
    
    if (ASYNC_MODE == emode) {
        _async_thread_ptr->async([&]() {
            this->process_rpc(ASYNC_MODE);
        });
        
    } else {
        _sync_thread_ptr->async([&]() {
            this->process_rpc(SYNC_MODE);
        });
    }
}

void RpcMgr::process_rpc(SocketMode emode) {
    StcpSocketPtr sock_ptr = get_connection(emode);
    FC_ASSERT(sock_ptr);
    sock_ptr->accept();
    
    if (ASYNC_MODE == emode) {
        _terminate_hello_loop_done = fc::async([&]() {
            send_hello_msg_loop();
        }, "send_hello_msg_loop");
        fc::async([&]() {
            read_loop(sock_ptr);
        }).wait();
    }
    
    return;
}
void RpcMgr::accept_loop() {
    int rpc_mode = MODE_COUNT;
    
    if (!_rpc_handler_ptr) {
        FC_THROW_EXCEPTION(lvm::global_exception::rpc_pointrt_null, \
                           "rpc process is null, please set the rpc processor. ");
    }
    
    while (true) {
        StcpSocketPtr sock_ptr = std::make_shared<StcpSocket>();
        
        try {
            _rpc_server.accept(sock_ptr->get_socket());
            //insert into container
            rpc_mode = insert_connection(sock_ptr);
            FC_ASSERT(rpc_mode < MODE_COUNT);
            process_connection((SocketMode)rpc_mode);
        }
        
        /*后续完善异常*/
        catch (fc::exception& e) {
            delete_connection((SocketMode)rpc_mode);
            elog("close connection ${e}", ("e", e.to_detail_string()));
        }
    }
}

void RpcMgr::set_endpoint(std::string& ip_addr, int port) {
    fc::ip::address ip(ip_addr);
    _end_point = fc::ip::endpoint(ip, port);
    _b_valid_flag = true;
    return;
}

int RpcMgr::insert_connection(StcpSocketPtr& sock) {
    int rpc_mode = ASYNC_MODE;
    _connection_mutex.lock();
    _rpc_connections.push_back(sock);
    rpc_mode = _rpc_connections.size() - 1;
    _connection_mutex.unlock();
    return rpc_mode;
}

void RpcMgr::delete_connection(SocketMode emode) {
    if (emode >= MODE_COUNT) {
        return;
    }
    
    _connection_mutex.lock();
    _rpc_connections[emode]->close();
    _rpc_connections.erase(_rpc_connections.begin() + uint32_t(emode));
    _connection_mutex.unlock();
    return;
}

void RpcMgr::close_connections() {
    std::vector<StcpSocketPtr>::iterator iter;
    _connection_mutex.lock();
    
    for (iter = _rpc_connections.begin(); iter != _rpc_connections.end(); iter++) {
        (*iter)->close();
        _rpc_connections.erase(iter);
    }
    
    _connection_mutex.unlock();
    return;
}

StcpSocketPtr RpcMgr::get_connection(SocketMode emode) {
    StcpSocketPtr tmp = NULL;
    _connection_mutex.lock();
    
    try {
        tmp = _rpc_connections.at(uint32_t(emode));
        
    } catch (std::exception& e) {
        tmp = NULL;
    }
    
    _connection_mutex.unlock();
    return tmp;
}

void RpcMgr::read_message(StcpSocketPtr& sock, std::string& msg_str) {
    uint64_t bytes_received = 0;
    uint64_t remaining_bytes_with_padding = 0;
    char* buffer_sock = NULL;
    MessageHeader m;
    char buffer[BUFFER_SIZE];
    int leftover = BUFFER_SIZE - sizeof(MessageHeader);
    /*first: read msgHead, get data.size*/
    sock->read(buffer, BUFFER_SIZE);
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
            sock->read(buffer_sock + BUFFER_SIZE, remaining_bytes_with_padding);
        }
        
    } catch (...) {
        delete[] buffer_sock;
        FC_THROW_EXCEPTION(lvm::global_exception::socket_read_error, \
                           "socket read error. ");
    }
    
    msg_str = std::string(buffer_sock, bytes_received);
    delete[] buffer_sock;
}

void RpcMgr::read_loop(StcpSocketPtr& sock) {
    std::string msg_str = "";
    bool b_need_restart = false;
    
    try {
        while (true) {
            read_message(sock, msg_str);
            _rpc_handler_ptr->handle_task(msg_str, nullptr);
        }
        
    } catch (lvm::global_exception::socket_read_error& e) {
        wlog("disconnected ${e}", ("e", e.to_detail_string()));
        b_need_restart = true;
        
    } catch (...) {
        b_need_restart = true;
    }
    
    if (b_need_restart) {
        b_need_restart = false;
        close_connections();
        _terminate_hello_loop_done.cancel();
    }
}

void RpcMgr::send_to_chain(Message& m, StcpSocketPtr& sock_ptr) {
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
        sock_ptr->write(padded_message.get(), size_with_padding);
        sock_ptr->flush();
        
    } catch (...) {
        elog("send message exception");
        FC_THROW_EXCEPTION(lvm::global_exception::socket_send_error, \
                           "socket send error. ");
    }
}
void RpcMgr::post_message(Message& rpc_msg) {
    StcpSocketPtr sock_ptr = NULL;
    
    //send response
    try {
        sock_ptr = get_connection(ASYNC_MODE);
        
        if (sock_ptr == NULL) {
            return;
        }
        
        send_to_chain(rpc_msg, sock_ptr);
        
    } catch (lvm::global_exception::socket_send_error& e) {
        delete_connection(ASYNC_MODE);
        FC_THROW_EXCEPTION(lvm::global_exception::async_socket_error, \
                           "async socket error: async send message error. ");
    }
}

//sync
void RpcMgr::send_message(TaskBase* task_p, std::string& resp) {
    FC_ASSERT(task_p != NULL);
    FC_ASSERT(task_p->task_type == LUA_REQUEST_TASK);
    FC_ASSERT(task_p->task_from == FROM_LUA_TO_CHAIN);
    StcpSocketPtr sock_ptr = NULL;
    sock_ptr = get_connection(SYNC_MODE);
    
    if (sock_ptr == NULL) {
        return;
    }
    
    //generate message
    LuaRequestTaskRpc task(*(LuaRequestTask*)task_p);
    Message m(task);
    
    //send msg
    try {
        send_to_chain(m, sock_ptr);
        read_message(sock_ptr, resp);
        
    } catch (lvm::global_exception::socket_send_error& e) {
        delete_connection(SYNC_MODE);
        FC_THROW_EXCEPTION(lvm::global_exception::sync_socket_error, \
                           "sync socket error: sync send message error. ");
                           
    } catch (lvm::global_exception::socket_read_error& e) {
        delete_connection(SYNC_MODE);
        FC_THROW_EXCEPTION(lvm::global_exception::sync_socket_error, \
                           "sync socket error: sync read message error. ");
    }
    
    return;
}

//send hello msg
void RpcMgr::send_hello_message() {
    uint32_t msg_len = 0;
    char* p_msg = nullptr;
    HelloMsgRpc hello_msg;
    hello_msg.data.task_from = FROM_RPC;
    hello_msg.data.task_type = HELLO_MSG;
    Message msg(hello_msg);
    msg_len = sizeof(MessageHeader) + msg.size;
    p_msg = new char[msg_len];
    memcpy(p_msg, (char*)&msg, sizeof(MessageHeader));
    memcpy(p_msg + sizeof(MessageHeader), msg.data.data(), msg.size);
    _rpc_handler_ptr->handle_task(std::string(p_msg, msg_len), nullptr);
    delete[] p_msg;
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
