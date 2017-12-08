#include <base/config.hpp>
#include <cli/method_data_handler.hpp>
#include <task/task.hpp>
#include <task/task_handler_base.hpp>
#include <glua/glua_task_mgr.h>

#include <boost/algorithm/string/trim.hpp>

#include <fc/io/json.hpp>

#include <memory>

TaskDispatcher*  TaskDispatcher::s_dispatcher = nullptr;

TaskDispatcher::TaskDispatcher()
    :_dispatch_task_thread("dispatch_task_thread") {
}

TaskDispatcher::~TaskDispatcher() {
    if (_dispatch_task_thread.is_running()) {
        _dispatch_task_thread.quit();
    }
    
    GluaTaskMgr::delete_glua_task_mgr();
}

TaskDispatcher* TaskDispatcher::get_dispatcher() {
    if (!TaskDispatcher::s_dispatcher) {
        TaskDispatcher::s_dispatcher = new TaskDispatcher;
    }
    
    return TaskDispatcher::s_dispatcher;
}

void TaskDispatcher::del_dispatcher() {
    if (TaskDispatcher::s_dispatcher) {
        delete TaskDispatcher::s_dispatcher;
        TaskDispatcher::s_dispatcher = nullptr;
    }
}

void TaskDispatcher::push_task(TaskBase* task_base,
                               TaskHandlerBase* call_back) {
    TaskAndCallback task;
    task.task_base = task_base;
    task.task_handler = call_back;
    {
        std::lock_guard<std::mutex> auto_guard(_task_mutex);
        _tasks.push_back(task);
    }
    dispatch_task();
}

void TaskDispatcher::dispatch_task_impl() {
    std::lock_guard<std::mutex> auto_guard(_task_mutex);
    auto iter = _tasks.begin();
    
    while (iter != _tasks.end()) {
        GluaTaskMgr* lua_task_mgr = GluaTaskMgr::get_glua_task_mgr();
        
        //  long-running operations
        //  sync  function call
        try {
            lua_task_mgr->execute_task(*iter);
            
        } catch (...) {
            //do nothing
        }
        
        TaskBase* task_base = iter->task_base;
        delete task_base;
        iter = _tasks.erase(iter);
    }
    
    _tasks.clear();
}

void TaskDispatcher::dispatch_task() {
    _dispatch_task_thread.schedule([this]() {
        dispatch_task_impl();
    },
    fc::time_point::now() + fc::seconds(DISPATCH_TASK_TIMESPAN),
    "dispatch the task");
}

TaskHandlerBase::~TaskHandlerBase() {
    TaskDispatcher::del_dispatcher();
}

void TaskHandlerBase::handle_task(const std::string& task,
                                  fc::buffered_istream* argument_stream) {
    TaskBase* task_base = parse_to_task(task, argument_stream);
    TaskDispatcher* dispatcher = TaskDispatcher::get_dispatcher();
    
    if (dispatcher) {
        dispatcher->push_task(task_base, this);
    }
}

LuaRequestTaskResult TaskHandlerBase::lua_request(LuaRequestTask& request_task) {
    LuaRequestTaskResult response_result;
    return response_result;
}

TaskBase* TaskHandlerBase::gen_compile_param_from_istream(
    fc::buffered_istream* argument_stream) {
    CompileTask* compile_task = new CompileTask;
    
    if (argument_stream) {
        std::string str_args;
        std::vector<char> arg_buffer(CLI_ARGS_MAX_CHARS);
        size_t real_read = argument_stream->readsome(&arg_buffer[0], CLI_ARGS_MAX_CHARS);
        
        if (real_read <= 0) {
            return compile_task;
        }
        
        if (arg_buffer[real_read] == char(0x04)) {
            str_args = std::string(&arg_buffer[0], real_read);
            boost::trim(str_args);
            compile_task->glua_path_file = fc::path(str_args);
        }
    }
    
    return compile_task;
}

TaskBase* TaskHandlerBase::gen_task_base_from_method_data(
    MethodData* method_data, fc::buffered_istream* argument_stream) {
    TaskBase* task_base = nullptr;
    
    if (!method_data) {
        return task_base;
    }
    
    if (method_data->name == "compile_contract") {
        task_base = gen_compile_param_from_istream(argument_stream);
        
    } else if (method_data->name == "register_contract") {
        task_base = new RegisterTask;
        
    } else if (method_data->name == "register_contract_testing") {
    } else if (method_data->name == "call_contract") {
        task_base = new CallTask;
        
    } else if (method_data->name == "call_contract_testing") {
    } else if (method_data->name == "upgrade_contract") {
    } else if (method_data->name == "destroy_contract") {
    }
    
    return task_base;
}
