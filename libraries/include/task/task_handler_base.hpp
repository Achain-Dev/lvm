/*
   author: pli
   date: 2017.10.17
   Handle the command line and message from chain.
   This class will parse the command line or message from chain and convert to  taskbase.
   It will cache the lua task, and it will run a lua thread in which lua will be compiled or called.
*/

#ifndef _TASK_HANDLER_BASE_H_
#define _TASK_HANDLER_BASE_H_

#include <base/misc.hpp>
#include <task/task.hpp>

#include <fc/io/buffered_iostream.hpp>
#include <fc/thread/thread.hpp>

#include <mutex>
#include <list>

struct MethodData;
class ITaskImplementFinishNotify;

class TaskHandlerBase : public ITaskImplementFinishNotify {
  public:
    TaskHandlerBase() {};
    virtual ~TaskHandlerBase();
    
    void handle_task(const std::string& task,
                     fc::buffered_istream* argument_stream);
                     
    virtual LuaRequestTaskResult lua_request(LuaRequestTask& request_task);
    
    virtual  void task_finished(TaskImplResult* result) = 0;
    
  protected:
    virtual  TaskBase* parse_to_task(const std::string& task,
                                     fc::buffered_istream* argument_stream) = 0;
    TaskBase* gen_task_base_from_method_data(MethodData* method_data,
            fc::buffered_istream* argument_stream);
            
  private:
    TaskBase* gen_compile_param_from_istream(
        fc::buffered_istream* argument_stream);
};

class TaskDispatcher {
  public:
    static TaskDispatcher* get_dispatcher();
    static void del_dispatcher();
    
    void push_task(TaskBase* task_base, TaskHandlerBase* call_back);
    
  private:
    TaskDispatcher();
    virtual ~TaskDispatcher();
    
    void dispatch_task();
    void dispatch_task_impl();
    
  private:
    std::list<TaskAndCallback>  _tasks;
    fc::thread              _dispatch_task_thread;
    std::mutex              _task_mutex;
    
  private:
    static TaskDispatcher*  s_dispatcher;
};

#endif
