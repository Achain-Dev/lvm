#include <base/exceptions.hpp>
#include <client/client.hpp>
#include <task/task_handler_base.hpp>

#include <glua/glua_chain_rpc_api.h>
#include <glua/glua_task_mgr.h>
#include <glua/lua_api.h>
#include <glua/glua_contractoperations.hpp>
#include <task/task.hpp>

#include <fc/exception/exception.hpp>

GluaTaskMgr*  GluaTaskMgr::_s_p_glua_task_mgr = nullptr;

GluaTaskMgr* GluaTaskMgr::get_glua_task_mgr() {
    if (!GluaTaskMgr::_s_p_glua_task_mgr) {
        GluaTaskMgr::_s_p_glua_task_mgr = new GluaTaskMgr;
    }
    
    return GluaTaskMgr::_s_p_glua_task_mgr;
}

void GluaTaskMgr::delete_glua_task_mgr() {
    if (GluaTaskMgr::_s_p_glua_task_mgr) {
        delete GluaTaskMgr::_s_p_glua_task_mgr;
        GluaTaskMgr::_s_p_glua_task_mgr = nullptr;
    }
}

GluaTaskMgr::GluaTaskMgr() {
    if (!lvm::lua::api::global_glua_chain_api) {
        lvm::lua::api::global_glua_chain_api =
            new lvm::lua::api::GluaChainRpcApi;
    }
}

GluaTaskMgr::~GluaTaskMgr() {
    if (lvm::lua::api::global_glua_chain_api) {
        delete lvm::lua::api::global_glua_chain_api;
        lvm::lua::api::global_glua_chain_api = nullptr;
    }
}

void GluaTaskMgr::execute_task(TaskAndCallback task) {
    // after execute task , gen the callback task then call back
    std::shared_ptr<TaskImplResult> sp_result;
    TaskImplResult* result = nullptr;
    std::shared_ptr<ContractOperation> contractop_ptr;
    _p_task_handler = task.task_handler;
    ContractOperation* op = BuildContractOperation(task.task_base);
    FC_ASSERT(op != nullptr);
    contractop_ptr.reset(op);
    
    if (contractop_ptr) {
        contractop_ptr->evaluate(task, &result);
        sp_result.reset(result);
    }
    
    FC_ASSERT(result);
    
    if (_p_task_handler) {
        _p_task_handler->task_finished(sp_result.get());
    }
    
    return;
}

LuaRequestTaskResult GluaTaskMgr::lua_request(LuaRequestTask& request_task) {
    LuaRequestTaskResult response_result;
    
    if (_p_task_handler) {
        response_result =_p_task_handler->lua_request(request_task);
    }
    
    return response_result;
}
