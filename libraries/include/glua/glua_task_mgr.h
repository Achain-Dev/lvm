/*
    author: pli
    date: 2017.10.26
    glua task manager
*/

#ifndef _GLUA_TASK_MGR_H_
#define _GLUA_TASK_MGR_H_

#include <base/misc.hpp>

struct TaskImplResult;

class GluaTaskMgr {
public:
    static GluaTaskMgr* get_glua_task_mgr();
    static void delete_glua_task_mgr();

    void execute_task(TaskAndCallback task);

    // sync call
    void lua_request(LuaRequestTask& request_task, std::string& response_data);

private:
    GluaTaskMgr();
    virtual ~GluaTaskMgr();

    TaskImplResult*  execute_compile_glua_file(TaskBase* task);

private:
    static GluaTaskMgr*  _s_p_glua_task_mgr;
    TaskHandlerBase*   _p_task_handler;
};

#endif
