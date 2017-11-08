#include <base/exceptions.hpp>
#include <glua/glua_contractoperations.hpp>
#include <glua/lua_lib.h>
#include <task/task.hpp>

#include <fc/exception/exception.hpp>

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
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    
    if (!_taskbase_ptr) {
        return;
    }
    
    RegisterTask* _registertask_ptr = (RegisterTask*)_taskbase_ptr;
    
    if (!_registertask_ptr) {
        return;
    }
    
    if (!result) {
        result = new RegisterTaskResult(_taskbase_ptr);
    }
    
    try {
        //
        using namespace lvm::lua::lib;
        GluaStateScope scope;
        GluaStateValue statevalue;
        size_t limit = _registertask_ptr->num_limit;
        statevalue.pointer_value =(void *) _registertask_ptr->statevalue;
        std::string str_tmp_caller = _registertask_ptr->str_caller;
        std::string str_tmp_caller_address = _registertask_ptr->str_caller_address;
        std::string str_tmp_contract_address = _registertask_ptr->str_contract_address;
        std::string str_tmp_args = "";
        std::string str_exception_msg = "";
        lvm::lua::api::global_glua_chain_api->clear_exceptions(scope.L());
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_address, statevalue, limit);
        scope.execute_contract_init_by_address(str_tmp_contract_address.c_str(), nullptr, nullptr);
        //
        int exception_code = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_code").int_value;
        
        if (exception_code > 0) {
            str_exception_msg = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
            
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
                
            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", str_exception_msg);
                throw con_err;
            }
        }
        
    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
        
    } catch (lvm::global_exception::contract_error& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
    }
}

void UpgradeContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const  {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    UpgradeTask* _upgradetask_ptr = (UpgradeTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_upgradetask_ptr) {
        return;
    }
    
    if (!result) {
        result = new UpgradeTaskResult(_taskbase_ptr);
    }
    
    try {
        using namespace lvm::lua::lib;
        GluaStateScope scope;
        GluaStateValue statevalue;
        size_t limit_num = _upgradetask_ptr->num_limit;
        std::string str_tmp_caller=_upgradetask_ptr->str_caller;
        std::string str_tmp_caller_addr = _upgradetask_ptr->str_caller_address;
        std::string str_tmp_method = "on_upgrade";
        std::string str_tmp_args ="";
        std::string str_tmp_contract_addr = _upgradetask_ptr->str_contract_address;
        statevalue.pointer_value = (void *)_upgradetask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
                                              
        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }
        
        int exception_code = 0;
        std::string exception_msg;
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        
        if (exception_code > 0) {
            exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
            
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
                
            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
                throw con_err;
            }
        }
        
    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
        
    } catch (lvm::global_exception::contract_error& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
    }
}

void DestroyContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    DestroyTask* _destroytask_ptr = (DestroyTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_destroytask_ptr) {
        return;
    }
    
    if (!result) {
        result = new DestroyTaskResult(_taskbase_ptr);
    }
    
    try {
        using namespace lvm::lua::lib;
        GluaStateScope scope;
        GluaStateValue statevalue;
        size_t limit_num = _destroytask_ptr->num_limit;
        std::string str_tmp_caller = _destroytask_ptr->str_caller;
        std::string str_tmp_caller_addr = _destroytask_ptr->str_caller_address;
        std::string str_tmp_method = "on_destroy";
        std::string str_tmp_args = "";
        std::string str_tmp_contract_addr = _destroytask_ptr->str_contract_address;
        statevalue.pointer_value = (void*)_destroytask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
                                              
        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }
        
        int exception_code = 0;
        std::string exception_msg;
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        
        if (exception_code > 0) {
            exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
            
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
                
            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
                throw con_err;
            }
        }
        
    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
        
    } catch (lvm::global_exception::contract_error& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
    }
}

void CallContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    CallTask* _calltask_ptr = (CallTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_calltask_ptr) {
        return;
    }
    
    if (!result) {
        result = new CallTaskResult(_taskbase_ptr);
    }
    
    try {
        using namespace lvm::lua::lib;
        GluaStateScope scope;
        GluaStateValue statevalue;
        size_t limit_num = _calltask_ptr->num_limit;
        std::string str_tmp_caller = _calltask_ptr->str_caller;
        std::string str_tmp_caller_addr = _calltask_ptr->str_caller_address;
        std::string str_tmp_method = _calltask_ptr->str_method;
        std::string str_tmp_args = _calltask_ptr->str_args;
        std::string str_tmp_contract_addr = _calltask_ptr->str_contract_address;
        statevalue.pointer_value = (void*)_calltask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
                                              
        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }
        
        int exception_code = 0;
        std::string exception_msg;
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        
        if (exception_code > 0) {
            exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
            
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
                
            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
                throw con_err;
            }
        }
        
        int left = limit_num - scope.get_instructions_executed_count();
        
    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
        
    } catch (lvm::global_exception::contract_error& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
    }
}

void TransferContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult* result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    TransferTask* _transfertask_ptr = (TransferTask*)_taskbase_ptr;
    
    if (!_taskbase_ptr || !_transfertask_ptr) {
        return;
    }
    
    if (!result) {
        result = new TransferTaskResult(_taskbase_ptr);
    }
    
    try {
        using namespace lvm::lua::lib;
        GluaStateScope scope;
        GluaStateValue statevalue;
        size_t limit_num = _transfertask_ptr->num_limit;
        std::string str_tmp_caller = _transfertask_ptr->str_caller;
        std::string str_tmp_caller_addr = _transfertask_ptr->str_caller_address;
        std::string str_tmp_method = "on_deposit";
        std::string str_tmp_args = _transfertask_ptr->str_args;
        std::string str_tmp_contract_addr = _transfertask_ptr->str_contract_address;
        statevalue.pointer_value = (void*)_transfertask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
                                              
        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }
        
        int exception_code = 0;
        std::string exception_msg;
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        
        if (exception_code > 0) {
            exception_msg = (char*)get_lua_state_value(scope.L(), "exception_msg").string_value;
            
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);
                
            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", exception_msg);
                throw con_err;
            }
        }
        
    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
        
    } catch (lvm::global_exception::contract_error& e) {
        result->error_msg = e.to_detail_string();
        result->error_code = e.code();
    }
}