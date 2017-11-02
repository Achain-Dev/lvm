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
    
    fc::tcp_server* get_server();
    
    void set_endpoint(std::string& ip_addr, int port);
    
    StcpSocketPtr get_connection();
    
    void insert_connection(StcpSocketPtr&);
    void delete_connection();
    void close_connections();
    void send_message(Message& rpc_msg);
    
  private:
    void accept_loop();
    void read_loop(StcpSocketPtr& sock);
    Message& generate_message(TaskImplResult* task);
    
    
  private:
    fc::tcp_server _rpc_server;
    fc::ip::endpoint _end_point;
    std::shared_ptr<fc::thread> _receive_msg_thread_ptr;
    //std::unordered_map<uint32_t, StcpSocketPtr> _rpc_connections;
    std::vector<StcpSocketPtr>      _rpc_connections;
    std::mutex              _connection_mutex;
    RpcTaskHandlerPtr _rpc_handler_ptr;
    Client* _client_ptr;
    bool _b_valid_flag;
    
    uint64_t _bytes_received;
    fc::time_point _connected_time;
    fc::time_point _last_message_received_time;
};

#endif
