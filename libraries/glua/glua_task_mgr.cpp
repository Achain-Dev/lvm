#include <base/exceptions.hpp>
#include <client/client.hpp>
#include <task/task_handler_base.hpp>

#include <glua/glua_chain_api.hpp>
#include <glua/glua_chain_rpc_api.h>
#include <glua/glua_complie_op.h>
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
    _p_task_handler = task.task_handler;
    
    switch (task.task_base->task_type) {
        case HELLO_MSG: {
            result = new HelloMsgResult();
            result->task_type = HELLO_MSG;
            result->task_from = FROM_RPC;
            break;
        }
        
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
        _contractop_ptr->evaluate(task, &result);
    }
    
    if (!result) {
        return;
    }
    
    if (_p_task_handler) {
        _p_task_handler->task_finished(result);
    }
    
    delete result;
    return;
}

LuaRequestTaskResult GluaTaskMgr::lua_request(LuaRequestTask& request_task) {
    LuaRequestTaskResult response_result;
    
    if (_p_task_handler) {
        response_result =_p_task_handler->lua_request(request_task);
    }
    
    return response_result;
}
