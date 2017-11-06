#include <base/exceptions.hpp>
#include <glua/glua_contractoperations.hpp>
#include <glua/lua_lib.h>
#include <task/task.hpp>

using lvm::lua::api::global_glua_chain_api;

static void setGluaStateScopeValue(lvm::lua::lib::GluaStateScope& scope,
                                   const std::string& str_caller,
                                   const std::string& str_caller_addr,
                                   const GluaStateValue& statevalue,
                                   const size_t limit_num) {
    lvm::lua::lib::add_global_string_variable(scope.L(), "caller", str_caller.c_str());
    lvm::lua::lib::add_global_string_variable(scope.L(), "caller_address", str_caller_addr.c_str());
    lvm::lua::lib::set_lua_state_value(scope.L(), "evaluate_state", statevalue, GluaStateValueType::LUA_STATE_VALUE_POINTER);
    global_glua_chain_api->clear_exceptions(scope.L());
    scope.set_instructions_limit(limit_num);
}

void RegisterContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const  {
}

void UpgradeContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const  {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    UpgradeTask* _upgradetask_ptr = (UpgradeTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_upgradetask_ptr) {
        return;
    }
    
    using namespace lvm::lua::lib;
    GluaStateScope scope;
    GluaStateValue statevalue;
    size_t limit_num = _upgradetask_ptr->num_limit;
    std::string str_tmp_caller=_upgradetask_ptr->str_caller;
    std::string str_tmp_caller_addr = _upgradetask_ptr->str_caller_address;
    std::string str_tmp_method = "on_upgrade";
    std::string str_tmp_args ="";
    std::string str_tmp_contract_addr = _upgradetask_ptr->str_contract_address;
    statevalue.pointer_value = nullptr;
    int exception_code = 0;
    std::string exception_msg;
    setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
    scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                          str_tmp_method.c_str(),
                                          str_tmp_args.c_str(),
                                          nullptr);
                                          
    if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
        FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
    }
    
    exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
    
    if (exception_code > 0) {
        exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
        
        if (exception_code == THINKYOUNG_API_LVM_LIMIT_OVER_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
            
        } else {
            lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
            throw con_err;
        }
    }
}

void DestroyContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    DestroyTask* _destroytask_ptr = (DestroyTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_destroytask_ptr) {
        return;
    }
    
    using namespace lvm::lua::lib;
    GluaStateScope scope;
    GluaStateValue statevalue;
    size_t limit_num = _destroytask_ptr->num_limit;
    std::string str_tmp_caller = _destroytask_ptr->str_caller;
    std::string str_tmp_caller_addr = _destroytask_ptr->str_caller_address;
    std::string str_tmp_method = "on_destroy";
    std::string str_tmp_args = "";
    std::string str_tmp_contract_addr = _destroytask_ptr->str_contract_address;
    statevalue.pointer_value = nullptr;
    int exception_code = 0;
    std::string exception_msg;
    setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
    scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                          str_tmp_method.c_str(),
                                          str_tmp_args.c_str(),
                                          nullptr);
                                          
    if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
        FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
    }
    
    exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
    
    if (exception_code > 0) {
        exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
        
        if (exception_code == THINKYOUNG_API_LVM_LIMIT_OVER_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
            
        } else {
            lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
            throw con_err;
        }
    }
}

void CallContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    CallTask* _calltask_ptr = (CallTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_calltask_ptr) {
        return;
    }
    
    using namespace lvm::lua::lib;
    GluaStateScope scope;
    GluaStateValue statevalue;
    size_t limit_num = _calltask_ptr->num_limit;
    std::string str_tmp_caller = _calltask_ptr->str_caller;
    std::string str_tmp_caller_addr = _calltask_ptr->str_caller_address;
    std::string str_tmp_method = _calltask_ptr->str_method;
    std::string str_tmp_args = _calltask_ptr->str_args;
    std::string str_tmp_contract_addr = _calltask_ptr->str_contract_address;
    statevalue.pointer_value = nullptr;
    int exception_code = 0;
    std::string exception_msg;
    setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
    scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                          str_tmp_method.c_str(),
                                          str_tmp_args.c_str(),
                                          nullptr);
                                          
    if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
        FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
    }
    
    exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
    
    if (exception_code > 0) {
        exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
        
        if (exception_code == THINKYOUNG_API_LVM_LIMIT_OVER_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
            
        } else {
            lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
            throw con_err;
        }
    }
    
    int left = limit_num - scope.get_instructions_executed_count();
}

void TransferContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    TransferTask* _transfertask_ptr = (TransferTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_transfertask_ptr) {
        return;
    }
    
    using namespace lvm::lua::lib;
    GluaStateScope scope;
    GluaStateValue statevalue;
    size_t limit_num = _transfertask_ptr->num_limit;
    std::string str_tmp_caller = _transfertask_ptr->str_caller;
    std::string str_tmp_caller_addr = _transfertask_ptr->str_caller_address;
    std::string str_tmp_method = "on_deposit";
    std::string str_tmp_args = _transfertask_ptr->str_args;
    std::string str_tmp_contract_addr = _transfertask_ptr->str_contract_address;
    statevalue.pointer_value = nullptr;
    int exception_code = 0;
    std::string exception_msg;
    setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
    scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                          str_tmp_method.c_str(),
                                          str_tmp_args.c_str(),
                                          nullptr);
                                          
    if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
        FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
    }
    
    exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
    
    if (exception_code > 0) {
        exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
        
        if (exception_code == THINKYOUNG_API_LVM_LIMIT_OVER_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
            
        } else {
            lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
            throw con_err;
        }
    }
}