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

#include <fc/io/buffered_iostream.hpp>
#include <fc/thread/thread.hpp>

#include <mutex>
#include <vector>

struct TaskBase;
struct MethodData;
class ITaskImplementFinishNotify;

class TaskDispatcher {
public:
    static TaskDispatcher* get_dispatcher();
    static void del_dispatcher();

    void push_task(TaskBase* task_base, ITaskImplementFinishNotify* call_back);

private:
    TaskDispatcher();
    virtual ~TaskDispatcher();

    void dispatch_task();
    void dispatch_task_impl();

private:
    std::vector<TaskAndCallback>  _tasks;
    fc::thread              _dispatch_task_thread;
    std::mutex			    _task_mutex;

private:
    static TaskDispatcher*  s_dispatcher;
};

class TaskHandlerBase : public ITaskImplementFinishNotify {
public:
    TaskHandlerBase() {};
    virtual ~TaskHandlerBase();

    void handle_task(const std::string& task,
        fc::buffered_istream* argument_stream);

protected:
    virtual  void task_finished(TaskImplResult* result) = 0;
    virtual  TaskBase* parse_to_task(const std::string& task,
        fc::buffered_istream* argument_stream) = 0;
    TaskBase* gen_task_base_from_method_data(MethodData* method_data,
        fc::buffered_istream* argument_stream);

private:
    TaskBase* gen_compile_param_from_istream(
        fc::buffered_istream* argument_stream);
};
#endif
