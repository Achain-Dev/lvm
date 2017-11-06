#include <base/exceptions.hpp>
#include <glua/GluaChainApi.hpp>
#include <glua/glua_complie_op.h>
#include <glua/glua_contractoperations.hpp>
#include <glua/glua_task_mgr.h>
#include <glua/thinkyoung_lua_api.h>
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
    if (!thinkyoung::lua::api::global_glua_chain_api) {
        thinkyoung::lua::api::global_glua_chain_api =
            new thinkyoung::lua::api::GluaChainApi;
    }
}

GluaTaskMgr::~GluaTaskMgr() {
    if (thinkyoung::lua::api::global_glua_chain_api) {
        delete thinkyoung::lua::api::global_glua_chain_api;
        thinkyoung::lua::api::global_glua_chain_api = nullptr;
    }
}

TaskImplResult* GluaTaskMgr::execute_compile_glua_file(TaskBase* task) {
    if (task->task_type != COMPILE_TASK) {
        return nullptr;
    }
    
    CompileOp compiler;
    CompileTask* compile_task = (CompileTask*)task;
    CompileTaskResult* result = new CompileTaskResult(task);
    fc::path glua_path = fc::path(compile_task->glua_path_file);
    
    try {
        fc::string gpc_path = compiler.compile_contract(glua_path).string();
        result->gpc_path_file = gpc_path.c_str() ? gpc_path.c_str() : "";
        
    } catch (const lvm::global_exception::contract_exception& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
        
    } catch (const fc::exception& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
    }
    
    return result;
}

void GluaTaskMgr::execute_task(TaskAndCallback task) {
    // TODO @xiaoming
    // after execute task , gen the callback task then call back
    TaskImplResult* result = nullptr;
    ContractOperation* _contractop_ptr = nullptr;
    
    switch (task.task_base->task_type) {
        case COMPILE_TASK:
            result = execute_compile_glua_file(task.task_base);
            break;
            
        case REGISTER_TASK:
            _contractop_ptr = new RegisterContractOperation();
            break;
            
        case UPGRADE_TASK:
            _contractop_ptr = new UpgradeContractOperation();
            break;
            
        case CALL_TASK:
            _contractop_ptr = new CallContractOperation();
            break;
            
        case TRANSFER_TASK:
            _contractop_ptr = new TransferContractOperation();
            break;
            
        case DESTROY_TASK:
            _contractop_ptr = new DestroyContractOperation();
            break;
            
        case TASK_COUNT:
        default:
            break;
    }
    
    if (_contractop_ptr) {
        _contractop_ptr->evaluate(task, result);
    }
    
    if (!result) {
        return;
    }
    
    if (task.call_back) {
        task.call_back->task_finished(result);
    }
    
    delete result;
    return;
}
