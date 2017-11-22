/*
author: saiy
date: 2017.10.17
rpc server
*/
#ifndef _RPC_MGR_H_
#define _RPC_MGR_H_

#include <fc/thread/thread.hpp>
#include <fc/network/ip.hpp>
#include <rpc/stcp_socket.hpp>
#include <rpc/rpc_task_handler.hpp>
#include <memory>
#include <iostream>

class Client;

class RpcMgr {
  public:
    RpcMgr(Client* client = nullptr);
    virtual ~RpcMgr();
    
    void start();
    
    void set_endpoint(std::string& ip_addr, int port);
    
    void close_connection();
    void post_message(Message& rpc_msg);
    void send_hello_msg_loop();
    
  private:
    void accept_loop();
    void read_loop();
    void send_hello_message();
    Message& generate_message(TaskImplResult* task);
    uint32_t read_message(std::string& msg_str);
    void send_to_chain(Message& m);;
    void process_rpc();
  private:
    fc::tcp_server _rpc_server;
    fc::ip::endpoint _end_point;
    fc::future<void>     _terminate_hello_loop_done;
    std::shared_ptr<fc::thread> _socket_thread_ptr;
    StcpSocket  _rpc_connection;
    RpcTaskHandlerPtr _rpc_handler_ptr;
    Client* _client_ptr;
    bool _b_valid_flag;
};

#endif
