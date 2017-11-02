/*
author: saiy
date: 2017.10.17
the handle of received rpc message, transfor string to task
*/

#ifndef _RPC_TASK_HANDLER_H_
#define _RPC_TASK_HANDLER_H_

#include <fc/thread/thread.hpp>
#include <base/config.hpp>
#include <rpc/rpc_msg.hpp>
#include <rpc/stcp_socket.hpp>
#include <task/task_handler_base.hpp>

class RpcMgr;

class RpcTaskHandler : public TaskHandlerBase {
  public:
    RpcTaskHandler(RpcMgr*);
    virtual ~RpcTaskHandler();
    
    void send_message(Message& msg);
    
  protected:
    virtual TaskBase* parse_to_task(const std::string& task,
                                    fc::buffered_istream* argument_stream);
    virtual void task_finished(TaskImplResult* result);
  private:
    Message generate_message(TaskImplResult* task_ptr);
    
  private:
    RpcMgr* _rpc_mgr_ptr;
    //luamgr* _lua_mgr_ptr;
    
    
};
typedef std::shared_ptr<RpcTaskHandler> RpcTaskHandlerPtr;

#endif