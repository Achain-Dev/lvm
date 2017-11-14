
#include <client/client.hpp>
#include <rpc/rpc_mgr.hpp>
#include <rpc/stcp_socket.hpp>
#include <rpc/rpc_msg.hpp>
#include <base/exceptions.hpp>
#include <iostream>


RpcMgr::RpcMgr(Client* client)
    :_receive_msg_thread_ptr(std::make_shared<fc::thread>("server")),
     _b_valid_flag(false),
     _rpc_handler_ptr(std::make_shared<RpcTaskHandler>(this)) {
    _client_ptr = client;
}

RpcMgr::~RpcMgr() {
    close_connections();
}

void RpcMgr::start() {
    if (!_b_valid_flag) {
        std::cout << "RpcMgr::start()  _valid_flag error " << std::endl;
        throw lvm::global_exception::rpc_exception();
    }
    
    _rpc_server.set_reuse_address();
    _rpc_server.listen(_end_point);
    _receive_msg_thread_ptr->async([&]() {
        this->accept_loop();
    });
}

void RpcMgr::set_endpoint(std::string& ip_addr, int port) {
    fc::ip::address ip(ip_addr);
    _end_point = fc::ip::endpoint(ip, port);
    _b_valid_flag = true;
    return;
}

fc::tcp_server* RpcMgr::get_server() {
    return &_rpc_server;
}


void RpcMgr::insert_connection(StcpSocketPtr& sock) {
    _connection_mutex.lock();
    _rpc_connections.push_back(sock);
    _connection_mutex.unlock();
    return;
}

void RpcMgr::delete_connection() {
    _connection_mutex.lock();
    _rpc_connections.pop_back();
    _connection_mutex.unlock();
    return;
}

void RpcMgr::close_connections() {
    std::vector<StcpSocketPtr>::iterator iter;
    _connection_mutex.lock();
    
    for (iter = _rpc_connections.begin(); iter != _rpc_connections.end(); iter++) {
        (*iter)->close();
    }
    
    _connection_mutex.unlock();
    return;
}

StcpSocketPtr RpcMgr::get_connection() {
    StcpSocketPtr tmp = NULL;
    _connection_mutex.lock();
    tmp = _rpc_connections.back();
    _connection_mutex.unlock();
    return tmp;
}

void RpcMgr::accept_loop() {
    if (!_rpc_handler_ptr) {
        std::cout << "RpcMgr::accept_loop error " << std::endl;
        return;
    }
    
    while (true) {
        StcpSocketPtr sock_ptr = std::make_shared<StcpSocket>();
        
        try {
            _rpc_server.accept(sock_ptr->get_socket());
            //insert into container
            insert_connection(sock_ptr);
            //do_key_exchange()
            sock_ptr->accept();
            //do read msg
            fc::async([&]() {
                read_loop(sock_ptr);
            }).wait();
        }
        
        /*后续完善异常*/
        catch (fc::exception& e) {
            sock_ptr->close();
            elog("close connection ${e}", ("e", e.to_detail_string()));
        }
    }
}
void RpcMgr::read_loop(StcpSocketPtr& sock) {
    const int BUFFER_SIZE = 16;
    const int LEFTOVER = BUFFER_SIZE - sizeof(MessageHeader);
    static_assert(BUFFER_SIZE >= sizeof(MessageHeader), "insufficient buffer");
    _connected_time = fc::time_point::now();
    fc::oexception exception_to_rethrow;
    bool call_on_connection_closed = false;
    
    try {
        MessageHeader m;
        
        while (true) {
            uint64_t bytes_received = 0;
            uint64_t remaining_bytes_with_padding = 0;
            char* buffer_sock = NULL;
            char buffer[BUFFER_SIZE];
            /*first: read msgHead, get data.size*/
            sock->read(buffer, BUFFER_SIZE);
            _bytes_received += BUFFER_SIZE;
            /*convert to MessageHeader*/
            memcpy((char*)&m, buffer, sizeof(MessageHeader));
            FC_ASSERT(m.size <= MAX_MESSAGE_SIZE, "", ("m.size", m.size)("MAX_MESSAGE_SIZE", MAX_MESSAGE_SIZE));
            /*the total len of the msg:(header + data)*/
            bytes_received = 16 * ((sizeof(MessageHeader) + m.size + 15) / 16);
            buffer_sock = new char[bytes_received];
            memset(buffer_sock, 0, bytes_received);
            memcpy(buffer_sock, buffer, BUFFER_SIZE);
            /*remaining len of byte to read from socket*/
            remaining_bytes_with_padding = 16 * ((m.size - LEFTOVER + 15) / 16);
            
            /*read the remain bytes*/
            if (remaining_bytes_with_padding) {
                sock->read(buffer_sock + BUFFER_SIZE, remaining_bytes_with_padding);
                _bytes_received += remaining_bytes_with_padding;
            }
            
            std::string msg_str(buffer_sock, bytes_received);
            _rpc_handler_ptr->handle_task(msg_str, nullptr);
            _last_message_received_time = fc::time_point::now();
            delete buffer_sock;
        }
        
    } catch (const fc::canceled_exception& e) {
        wlog("caught a canceled_exception in read_loop.  this should mean we're in the process of deleting this object already, so there's no need to notify the delegate: ${e}", ("e", e.to_detail_string()));
        throw;
        
    } catch (const fc::eof_exception& e) {
        wlog("disconnected ${e}", ("e", e.to_detail_string()));
        call_on_connection_closed = true;
        
    } catch (const fc::exception& e) {
        elog("disconnected ${er}", ("er", e.to_detail_string()));
        call_on_connection_closed = true;
        exception_to_rethrow = fc::unhandled_exception(FC_LOG_MESSAGE(warn, "disconnected: ${e}", ("e", e.to_detail_string())));
        
    } catch (const std::exception& e) {
        elog("disconnected ${er}", ("er", e.what()));
        call_on_connection_closed = true;
        exception_to_rethrow = fc::unhandled_exception(FC_LOG_MESSAGE(warn, "disconnected: ${e}", ("e", e.what())));
        
    } catch (...) {
        elog("unexpected exception");
        call_on_connection_closed = true;
        exception_to_rethrow = fc::unhandled_exception(FC_LOG_MESSAGE(warn, "disconnected: ${e}", ("e", fc::except_str())));
    }
    
    if (call_on_connection_closed) {
        delete_connection();
    }
    
    if (exception_to_rethrow)
        throw *exception_to_rethrow;
}

void RpcMgr::send_message(Message& rpc_msg) {
    uint32_t size_of_message_and_header = 0;
    uint32_t size_with_padding = 0;
    StcpSocketPtr sock_ptr = NULL;
    //padding rpc data
    size_of_message_and_header = sizeof(MessageHeader) + rpc_msg.size;
    //pad the message we send to a multiple of 16 bytes
    size_with_padding = 16 * ((size_of_message_and_header + 15) / 16);
    std::unique_ptr<char[]> padded_message(new char[size_with_padding]);
    memcpy(padded_message.get(), (char*)&rpc_msg, sizeof(MessageHeader));
    memcpy(padded_message.get() + sizeof(MessageHeader), rpc_msg.data.data(), rpc_msg.size);
    
    //send response
    try {
        sock_ptr = get_connection();
        FC_ASSERT(sock_ptr != NULL);
        sock_ptr->write(padded_message.get(), size_with_padding);
        sock_ptr->flush();
        
    } catch (fc::exception& er) {
        er;
        //TODO
    } catch (const std::exception& e) {
        e;
        //TODO
    } catch (...) {
        //TODO
    }
}