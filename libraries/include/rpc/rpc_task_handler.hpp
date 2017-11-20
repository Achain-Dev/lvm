/*
author: saiy
date: 2017.10.17
the handle of received rpc message, transfor string to task
*/

#ifndef _RPC_TASK_HANDLER_H_
#define _RPC_TASK_HANDLER_H_

#include <fc/thread/thread.hpp>
#include <fc/thread/future.hpp>
#include <base/config.hpp>
#include <rpc/rpc_msg.hpp>
#include <rpc/stcp_socket.hpp>
#include <task/task_handler_base.hpp>

class RpcMgr;

class RpcTaskHandler : public TaskHandlerBase {
  public:
    RpcTaskHandler(RpcMgr*);
    virtual ~RpcTaskHandler();
    Message generate_message(TaskImplResult* task_ptr);
    void post_message(Message& msg);
    
  protected:
    virtual TaskBase* parse_to_task(const std::string& task,
                                    fc::buffered_istream* argument_stream);
    virtual void task_finished(TaskImplResult* result);
    
    virtual LuaRequestTaskResult lua_request(LuaRequestTask& request_task);
    
  private:
    RpcMgr* _rpc_mgr_ptr;
    fc::promise<void*>::ptr _lua_request_promise_ptr;
};
typedef std::shared_ptr<RpcTaskHandler> RpcTaskHandlerPtr;

#endif