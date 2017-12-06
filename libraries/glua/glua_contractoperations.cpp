#include <base/exceptions.hpp>
#include <glua/glua_complie_op.h>
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

static void store_contractinfo_in_chain(lvm::lua::lib::GluaStateScope& scope,
                                        const std::string& str_contract_address,
                                        const std::string& str_contract_id,
                                        Code& _code) {
    GluaStateValue value;
    value.string_value = str_contract_address.c_str ();
    lvm::lua::lib::set_lua_state_value(scope.L(), STR_CONTRACT_ADDRESS_IN_CHAIN, value, LUA_STATE_VALUE_STRING);
    value.string_value = str_contract_id.c_str();
    lvm::lua::lib::set_lua_state_value(scope.L(), STR_CONTRACT_ID_IN_CHAIN, value, LUA_STATE_VALUE_STRING);
    value.pointer_value = reinterpret_cast<void*>(& _code);
    lvm::lua::lib::set_lua_state_value(scope.L(), PTR_ENTRY_CODE_IN_CHAIN, value, GluaStateValueType::LUA_STATE_VALUE_POINTER);
}

// ContractOperation's factory, generate the ContractOperation object's point.
// the point's life time be managed by caller.
ContractOperation* BuildContractOperation(TaskBase* task) {
    FC_ASSERT(task != nullptr);

    switch (task->task_type) {
        case HELLO_MSG:
            return new HelloMsgOperation();

        case COMPILE_TASK:
            return new CompileContractOperation();

        case REGISTER_TASK:
            return new RegisterContractOperation();
            break;

        case UPGRADE_TASK:
            return new UpgradeContractOperation();

        case CALL_TASK:
            return new CallContractOperation();

        case TRANSFER_TASK:
            return new TransferContractOperation();

        case DESTROY_TASK:
            return new DestroyContractOperation();

        case COMPILE_SCRIPT_TASK:
            return new CompileScriptOperation();

        case HANDLE_EVENTS_TASK:
            return new HandleEventsOperation();

        case CALL_OFFLINE_TASK:
            return new CallContractOfflineOperation();

        default:
            return nullptr;
    }
}

void RegisterContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const  {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;

    if (!_taskbase_ptr) {
        return;
    }

    RegisterTask* _registertask_ptr = (RegisterTask*)_taskbase_ptr;

    if (!_registertask_ptr) {
        return;
    }

    if (!*result) {
        *result = new RegisterTaskResult(_taskbase_ptr);
    }

    try {
        //
        using namespace lvm::lua::lib;
        GluaStateScope scope;
        GluaStateValue statevalue;
        size_t limit = _registertask_ptr->num_limit;
        statevalue.pointer_value =(void *) _registertask_ptr->statevalue;
        std::string str_tmp_caller = _registertask_ptr->str_caller;
        std::string str_tmp_caller_addr = _registertask_ptr->str_caller_address;
        std::string str_tmp_contract_addr = _registertask_ptr->str_contract_address;
        std::string str_tmp_contract_id = _registertask_ptr->str_contract_id;
        Code _code = _registertask_ptr->contract_code;
        std::string str_tmp_args = "";
        std::string str_exception_msg = "";
        lvm::lua::api::global_glua_chain_api->clear_exceptions(scope.L());
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit);
        store_contractinfo_in_chain(scope, str_tmp_contract_addr, str_tmp_contract_id, _code);
        scope.execute_contract_init_by_address(str_tmp_contract_addr.c_str(), nullptr, nullptr);
        //
        int exception_code = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_code").int_value;
        str_exception_msg = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
        (*result)->execute_count = scope.get_instructions_executed_count();
        (*result)->error_code = exception_code;
        (*result)->error_msg = str_exception_msg;

        if (exception_code > 0) {
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);

            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", str_exception_msg);
                throw con_err;
            }
        }

    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();

    } catch (lvm::global_exception::contract_error& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();
    }
}

void UpgradeContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const  {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    UpgradeTask* _upgradetask_ptr = (UpgradeTask*)_taskbase_ptr;

    if (!_taskbase_ptr || !_upgradetask_ptr) {
        return;
    }

    if (!*result) {
        *result = new UpgradeTaskResult(_taskbase_ptr);
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
        std::string str_tmp_contract_id = _upgradetask_ptr->str_contract_id;
        Code _code = _upgradetask_ptr->contract_code;
        statevalue.pointer_value = (void *)_upgradetask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        store_contractinfo_in_chain(scope, str_tmp_contract_addr, str_tmp_contract_id, _code);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
        int exception_code = 0;
        std::string str_exception_msg("");
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        str_exception_msg = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
        (*result)->execute_count = scope.get_instructions_executed_count();
        (*result)->error_code = exception_code;
        (*result)->error_msg = str_exception_msg;

        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }

        if (exception_code > 0) {
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);

            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", str_exception_msg);
                throw con_err;
            }
        }

    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();

    } catch (lvm::global_exception::contract_error& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();
    }
}

void DestroyContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    DestroyTask* _destroytask_ptr = (DestroyTask*)_taskbase_ptr;

    if (!_taskbase_ptr || !_destroytask_ptr) {
        return;
    }

    if (!*result) {
        *result = new DestroyTaskResult(_taskbase_ptr);
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
        std::string str_tmp_contract_id = _destroytask_ptr->str_contract_id;
        Code _code = _destroytask_ptr->contract_code;
        statevalue.pointer_value = (void*)_destroytask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        store_contractinfo_in_chain(scope, str_tmp_contract_addr, str_tmp_contract_id, _code);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
        int exception_code = 0;
        std::string str_exception_msg("");
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        str_exception_msg = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
        (*result)->execute_count = scope.get_instructions_executed_count();
        (*result)->error_code = exception_code;
        (*result)->error_msg = str_exception_msg;

        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }

        if (exception_code > 0) {
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);

            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", str_exception_msg);
                throw con_err;
            }
        }

    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();

    } catch (lvm::global_exception::contract_error& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();
    }
}

void CallContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    CallTask* _calltask_ptr = (CallTask*)_taskbase_ptr;

    if (!_taskbase_ptr || !_calltask_ptr) {
        return;
    }

    if (!*result) {
        *result = new CallTaskResult(_taskbase_ptr);
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
        std::string str_tmp_contract_id = _calltask_ptr->str_contract_id;
        Code _code = _calltask_ptr->contract_code;
        statevalue.pointer_value = (void*)_calltask_ptr->statevalue;
        setGluaStateScopeValue(scope, str_tmp_caller, str_tmp_caller_addr, statevalue, limit_num);
        store_contractinfo_in_chain(scope, str_tmp_contract_addr, str_tmp_contract_id, _code);
        scope.execute_contract_api_by_address(str_tmp_contract_addr.c_str(),
                                              str_tmp_method.c_str(),
                                              str_tmp_args.c_str(),
                                              nullptr);
        int exception_code = 0;
        std::string str_exception_msg("");
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        str_exception_msg = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
        (*result)->execute_count = scope.get_instructions_executed_count();
        (*result)->error_code = exception_code;
        (*result)->error_msg = str_exception_msg;

        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }

        if (exception_code > 0) {
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);

            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", str_exception_msg);
                throw con_err;
            }
        }

    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();

    } catch (lvm::global_exception::contract_error& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();
    }
}

void TransferContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    TaskBase* _taskbase_ptr = _inst_taskandcallback.task_base;
    TransferTask* _transfertask_ptr = (TransferTask*)_taskbase_ptr;

    if (!_taskbase_ptr || !_transfertask_ptr) {
        return;
    }

    if (!*result) {
        *result = new TransferTaskResult(_taskbase_ptr);
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
        int exception_code = 0;
        std::string str_exception_msg("");
        exception_code = get_lua_state_value(scope.L(), "exception_code").int_value;
        str_exception_msg = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
        (*result)->execute_count = scope.get_instructions_executed_count();
        (*result)->error_code = exception_code;
        (*result)->error_msg = str_exception_msg;

        if (scope.L()->force_stopping == true && scope.L()->exit_code == LUA_API_INTERNAL_ERROR) {
            FC_CAPTURE_AND_THROW(lvm::global_exception::lua_executor_internal_error, (""));
        }

        if (exception_code > 0) {
            if (exception_code == LVM_API_LVM_LIMIT_OVER_ERROR) {
                FC_CAPTURE_AND_THROW(lvm::global_exception::contract_run_out_of_money);

            } else {
                lvm::global_exception::contract_error con_err(32000, "exception", str_exception_msg);
                throw con_err;
            }
        }

    } catch (lvm::global_exception::contract_run_out_of_money& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();

    } catch (lvm::global_exception::contract_error& e) {
        (*result)->error_msg = e.to_detail_string();
        (*result)->error_code = e.code();
    }
}

void CompileContractOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    FC_ASSERT(_inst_taskandcallback.task_base->task_type == COMPILE_TASK);
    CompileTask* task = (CompileTask*)_inst_taskandcallback.task_base;
    *result = new CompileTaskResult(task);
    CompileTaskResult* compile_result = (CompileTaskResult*)(*result);
    fc::path glua_path = task->glua_path_file;

    try {
        CompileOp compiler;
        fc::string gpc_path = compiler.compile_lua(glua_path, true).string();
        compile_result->gpc_path_file = gpc_path.c_str() ? gpc_path.c_str() : "";

    } catch (const lvm::global_exception::contract_exception& e) {
        compile_result->error_msg = e.to_detail_string();
        compile_result->error_code = e.code();

    } catch (const fc::exception& e) {
        compile_result->error_msg = e.to_detail_string();
        compile_result->error_code = e.code();
    }
}

void CompileScriptOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    FC_ASSERT(_inst_taskandcallback.task_base->task_type == COMPILE_SCRIPT_TASK);
    CompileScriptTask* task = (CompileScriptTask*)_inst_taskandcallback.task_base;
    fc::path filename = task->path_file_name;
    *result = new CompileScriptTaskResult(task);
    CompileScriptTaskResult* compile_script_result = (CompileScriptTaskResult*)(*result);

    try {
        CompileOp compiler;
        fc::string gpc_path = compiler.compile_lua(task->path_file_name, false).string();
        compile_script_result->script_path_file = gpc_path.c_str() ? gpc_path.c_str() : "";

    } catch (const lvm::global_exception::contract_exception& e) {
        compile_script_result->error_msg = e.to_detail_string();
        compile_script_result->error_code = e.code();

    } catch (const fc::exception& e) {
        compile_script_result->error_msg = e.to_detail_string();
        compile_script_result->error_code = e.code();
    }
};

void HandleEventsOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    FC_ASSERT(_inst_taskandcallback.task_base->task_type == HANDLE_EVENTS_TASK);
    HandleEventsTask* task = (HandleEventsTask*)_inst_taskandcallback.task_base;
    *result = new HandleEventsTaskResult(task);
    HandleEventsTaskResult* handle_event_result = (HandleEventsTaskResult*)(*result);
    bool is_truncated = task->is_truncated;
    std::string event_type = task->event_type;
    std::string event_param = task->event_param;
    std::string contract_id = task->contract_id;
    Code code_stream = task->script_code;

    try {
        lvm::lua::lib::GluaStateScope scope;
        lvm::lua::lib::add_global_bool_variable(scope.L(), "truncated", is_truncated);
        lvm::lua::lib::add_global_string_variable(scope.L(), "event_type", event_type.c_str());
        lvm::lua::lib::add_global_string_variable(scope.L(), "param", event_param.c_str());
        lvm::lua::lib::add_global_string_variable(scope.L(), "contract_id", contract_id.c_str());
        lvm::lua::lib::run_compiled_bytestream(scope.L(), &code_stream);

    } catch (const lvm::global_exception::contract_exception& e) {
        handle_event_result->error_code = e.code();
        handle_event_result->error_msg = e.to_detail_string();

    } catch (const fc::exception& e) {
        handle_event_result->error_code = e.code();
        handle_event_result->error_msg = e.to_detail_string();
    }
};

void CallContractOfflineOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    FC_ASSERT(_inst_taskandcallback.task_base->task_type == CALL_OFFLINE_TASK);
    CallContractOfflineTask* task = (CallContractOfflineTask*)_inst_taskandcallback.task_base;
    *result = new CallContractOfflineTaskResult(task);
    CallContractOfflineTaskResult* call_offline_result = (CallContractOfflineTaskResult*)(*result);

    try {
        lvm::lua::lib::GluaStateScope scope;
        GluaStateValue statevalue;
        statevalue.pointer_value = (void*)task->statevalue;
        lvm::lua::lib::add_global_string_variable(scope.L(), "caller", task->str_caller.c_str());
        lvm::lua::lib::add_global_string_variable(scope.L(), "caller_address", task->str_caller_address.c_str());
        lvm::lua::lib::set_lua_state_value(scope.L(), "evaluate_state", statevalue, GluaStateValueType::LUA_STATE_VALUE_POINTER);
        lvm::lua::api::global_glua_chain_api->clear_exceptions(scope.L());
        scope.set_instructions_limit(task->num_limit);
        std::string json_result;
        scope.execute_contract_api_by_address(task->str_contract_id.c_str(), task->str_method.c_str(), task->str_args.c_str(), &json_result);
        int exception_code = lvm::lua::lib::get_lua_state_value(scope.L(), "exception_code").int_value;
        char* exception_msg = (char*)lvm::lua::lib::get_lua_state_value(scope.L(), "exception_msg").string_value;
        call_offline_result->json_string = json_result;
        call_offline_result->error_code = exception_code;
        call_offline_result->error_msg = exception_msg ? exception_msg : "";

    } catch (const lvm::global_exception::contract_exception& e) {
        call_offline_result->error_msg = e.to_detail_string();
        call_offline_result->error_code = e.code();

    } catch (const fc::exception& e) {
        call_offline_result->error_msg = e.to_detail_string();
        call_offline_result->error_code = e.code();
    }
};

void HelloMsgOperation::evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const {
    FC_ASSERT(_inst_taskandcallback.task_base->task_type == HELLO_MSG);
    *result = new HelloMsgResult();
    (*result)->task_type = HELLO_MSG;
    (*result)->task_from = FROM_RPC;
}
