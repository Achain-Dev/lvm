
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
    }).wait();
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

Message* RpcMgr::generate_message(void* task_ptr) {
    FC_ASSERT(task_ptr != NULL);
    TaskBase* task_base_prt = NULL;
    Message* msg_ptr;
    //get task_type
    task_base_prt = (TaskBase*)task_ptr;
    
    switch (task_base_prt->task_type) {
        case COMPILE_TASK_EXE_RESULT: {
#if 0
            //GRbit 的代码，先放在这里
            CompileTaskRpc compile_rpc(*(CompileTask*)task_ptr);
            msg_ptr = new Message(compile_rpc);
            //set msg_id from CompileTask
            msg_ptr->msg_id = compile_rpc.data.task_id;
#endif
            CompileTaskResultRpc reslut(*(CompileTaskResult*)task_ptr);
            msg_ptr = new Message(reslut);
            msg_ptr->msg_id = reslut.data.task_id;
            break;
        }
        
        case CALL_TASK: {
            CallTaskRpc call_rpc(*(CallTask*)task_ptr);
            msg_ptr = new Message(call_rpc);
            //set msg_id from CallTask
            msg_ptr->msg_id = call_rpc.data.task_id;
            break;
        }
        
        case REGISTER_TASK: {
            RegisterTaskRpc register_rpc(*(RegisterTask*)task_ptr);
            msg_ptr = new Message(register_rpc);
            //set msg_id from CallTask
            msg_ptr->msg_id = register_rpc.data.task_id;
            break;
        }
        
        default: {
            FC_THROW_EXCEPTION(lvm::global_exception::task_type_error, \
                               "the msg_type of task response error " \
                               "$ {msg_type}", ("msg_type", task_base_prt->task_type));
            break;
        }
    }
    
    return msg_ptr;
}

void RpcMgr::send_message(void* task_ptr) {
    FC_ASSERT(task_ptr != NULL);
    uint32_t size_of_message_and_header = 0;
    uint32_t size_with_padding = 0;
    uint32_t msg_id = 0;
    Message* msg_ptr = NULL;
    StcpSocketPtr sock_ptr = NULL;
    
    //get Message from task
    try {
        //the memery of msg_ptr is new in generate_message,
        //delete the memery here
        msg_ptr = generate_message(task_ptr);
        FC_ASSERT(msg_ptr != NULL);
        
    } catch (lvm::global_exception::task_type_error) {
        //TODO
        delete msg_ptr;
    }
    
    //padding rpc data
    size_of_message_and_header = sizeof(MessageHeader) + msg_ptr->size;
    //pad the message we send to a multiple of 16 bytes
    size_with_padding = 16 * ((size_of_message_and_header + 15) / 16);
    std::unique_ptr<char[]> padded_message(new char[size_with_padding]);
    memcpy(padded_message.get(), (char*)msg_ptr, sizeof(MessageHeader));
    memcpy(padded_message.get() + sizeof(MessageHeader), msg_ptr->data.data(), msg_ptr->size);
    //delete msg_ptr
    msg_id = msg_ptr->msg_id;
    delete msg_ptr;
    
    //send response
    try {
        sock_ptr = get_connection();
        FC_ASSERT(sock_ptr != NULL);
        sock_ptr->write(padded_message.get(), size_with_padding);
        sock_ptr->flush();
        //close this connection and release it
        sock_ptr->close();
        delete_connection();
        
    } catch (fc::exception& er) {
        //TODO
    } catch (const std::exception& e) {
        //TODO
    } catch (...) {
        //TODO
    }
}