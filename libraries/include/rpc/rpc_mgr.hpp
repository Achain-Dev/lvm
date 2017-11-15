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
    
    void set_endpoint(std::string& ip_addr, int port, SocketMode emode);
    
    StcpSocketPtr get_connection(SocketMode emode);
    
    void close_connections();
    void post_message(Message& rpc_msg);
    void send_hello_msg_loop();
    void send_message(TaskBase* task_p, std::string& resp);
    
  private:
    void accept_loop(SocketMode emode);
    void read_loop(StcpSocketPtr& sock);
    void send_hello_message();
    Message& generate_message(TaskImplResult* task);
    void read_message(StcpSocketPtr& sock, std::string& msg_str);
    void insert_connection(StcpSocketPtr& sock, SocketMode emode);
    void delete_connection(SocketMode emode);
    void send_to_chain(Message& m, StcpSocketPtr& sock);
    
    
  private:
    fc::tcp_server _rpc_server[MODE_COUNT];
    fc::ip::endpoint _end_point[MODE_COUNT];
    fc::future<void>     _terminate_hello_loop_done;
    std::shared_ptr<fc::thread> _sync_thread_ptr;
    std::shared_ptr<fc::thread> _async_thread_ptr;
    std::vector<StcpSocketPtr>      _rpc_connections;
    std::mutex              _connection_mutex;
    RpcTaskHandlerPtr _rpc_handler_ptr;
    Client* _client_ptr;
    bool _b_valid_flag[MODE_COUNT];
};

#endif
