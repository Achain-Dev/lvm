/*
   author: pli
   date: 2017.10.17
   contract task entity
*/
#ifndef _TASK_H_
#define _TASK_H_

#include <glua/glua_contractentry.hpp>
#include <glua/lua_lib.h>

#include <fc/filesystem.hpp>

#include <stdint.h>
#include <string>

struct Message;

enum LUA_TASK_TYPE {
    COMPILE_TASK = 0,
    COMPILE_TASK_RESULT,
    COMPILE_SCRIPT_TASK,
    COMPILE_SCRIPT_RESULT,
    REGISTER_TASK,
    REGISTER_TASK_RESULT,
    UPGRADE_TASK,
    UPGRADE_TASK_RESULT,
    CALL_TASK,
    CALL_TASK_RESULT,
    CALL_OFFLINE_TASK,
    CALL_OFFLINE_TASK_RESULT,
    TRANSFER_TASK,
    TRANSFER_TASK_RESULT,
    DESTROY_TASK,
    DESTROY_TASK_RESULT,
    HANDLE_EVENTS_TASK,
    HANDLE_EVENTS_TASK_RESULT,
    LUA_REQUEST_TASK,
    LUA_REQUEST_RESULT_TASK,
    HELLO_MSG,
    TASK_COUNT
};

enum LUA_TASK_FROM {
    FROM_CLI = 0,
    FROM_RPC,
    FROM_LUA_TO_CHAIN,
    FROM_COUNT
};

enum LUA_REQUEST_METHOD {
    GET_STORED_CONTRACT_INFO_BY_ADDRESS = 0,
    GET_CONTRACT_ADDRESS_BY_NAME,
    CHECK_CONTRACT_EXIST_BY_ADDRESS,
    CHECK_CONTRACT_EXIST,
    OPEN_CONTRACT,
    OPEN_CONTRACT_BY_ADDRESS,
    GET_STORAGE_VALUE_FROM_THINKYOUNG,
    GET_CONTRACT_BALANCE_AMOUNT,
    GET_TRANSACTION_FEE,
    GET_CHAIN_NOW,
    GET_CHAIN_RANDOM,
    GET_TRANSACTION_ID,
    GET_HEADER_BLOCK_NUM,
    WAIT_FOR_FUTURE_RANDOM,
    GET_WAITED,
    COMMIT_STORAGE_CHANGES_TO_THINKYOUNG,
    TRANSFER_FROM_CONTRACT_TO_ADDRESS,
    TRANSFER_FROM_CONTRACT_TO_PUBLIC_ACCOUNT,
    EMIT,
    COUNT
};

struct TaskBase {
    TaskBase();

    uint32_t task_id;     //a random value,CLI or achain launch a request with a task_id
    LUA_TASK_TYPE task_type;   //here,change LUA_TASK_TYPE to uint32_t, fit FC name
    LUA_TASK_FROM task_from;    //LUA_TASK_FORM_CLI LUA_TASK_FORM_RPC
};

struct TaskImplResult : public TaskBase {
    TaskImplResult();
    virtual ~TaskImplResult() {};

    void   init_task_base(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
  public:
    uint64_t      error_code;
    uint64_t      execute_count;
    std::string   error_msg;
    std::string   json_string;
};

//hello msg
typedef struct TaskBase HelloMsg;
typedef struct TaskImplResult HelloMsgResult;

struct CompileTaskResult : public TaskImplResult {
    CompileTaskResult() {}
    CompileTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();

    std::string  gpc_path_file;
};

struct RegisterTaskResult : public TaskImplResult {
    RegisterTaskResult() {}
    RegisterTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();

    //TODO
};

struct CallTaskResult : public TaskImplResult {
    CallTaskResult() {}
    CallTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
    //TODO
};

struct TransferTaskResult : public TaskImplResult {
    TransferTaskResult() {}
    TransferTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
    //TODO
};

struct UpgradeTaskResult : public TaskImplResult {
    UpgradeTaskResult() {}
    UpgradeTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
    //TODO
};

struct DestroyTaskResult : public TaskImplResult {
    DestroyTaskResult() {}
    DestroyTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
    //TODO
};

struct CompileScriptTaskResult  : TaskImplResult {
    CompileScriptTaskResult() {}
    CompileScriptTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();

    std::string  script_path_file;
};

struct HandleEventsTaskResult : TaskImplResult {
    HandleEventsTaskResult() {}
    HandleEventsTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
};

struct CallContractOfflineTaskResult : TaskImplResult {
    CallContractOfflineTaskResult() {}
    CallContractOfflineTaskResult(TaskBase* task);

    virtual  std::string  get_result_string();
    virtual  Message get_rpc_message();
};

//task
struct CompileTask : public TaskBase {
    CompileTask() {
        task_type = COMPILE_TASK;
    };

    CompileTask(const CompileTask& task) {
        if (this != &task) {
            task_type = COMPILE_TASK;
            task_id = task.task_id;
            task_from = task.task_from;
            glua_path_file = task.glua_path_file;
        }
    };

    fc::path glua_path_file;
};

struct RegisterTask : public TaskBase {
    RegisterTask() {
        task_type = REGISTER_TASK;
    };

    RegisterTask(const RegisterTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = REGISTER_TASK;
            contract_code = task.contract_code;
            statevalue = task.statevalue;
            num_limit = task.num_limit;
            gpc_code = task.gpc_code;
            str_caller = task.str_caller;
            str_caller_address = task.str_caller_address;
            str_contract_address = task.str_contract_address;
            str_contract_id = task.str_contract_id;
        }
    };

    std::string             gpc_code;
    intptr_t                statevalue;
    int                     num_limit;
    std::string             str_caller;
    std::string             str_caller_address;
    std::string             str_contract_address;
    std::string             str_contract_id;
    Code                    contract_code;
};

struct UpgradeTask : public TaskBase {
    UpgradeTask() {
        task_type = UPGRADE_TASK;
    };

    UpgradeTask(const UpgradeTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = UPGRADE_TASK;
            contract_code = task.contract_code;
            statevalue = task.statevalue;
            num_limit = task.num_limit;
            str_caller = task.str_caller;
            str_caller_address = task.str_caller_address;
            str_contract_address = task.str_contract_address;
            str_contract_id = task.str_contract_id;
        }
    }

    intptr_t                statevalue;
    int                     num_limit;
    std::string             str_caller;
    std::string             str_caller_address;
    std::string             str_contract_address;
    std::string             str_contract_id;
    Code                    contract_code;
};

struct CallTask : public TaskBase {
    CallTask() {
        task_type = CALL_TASK;
    };
    CallTask(const CallTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = CALL_TASK;
            contract_code = task.contract_code;
            statevalue = task.statevalue;
            num_limit = task.num_limit;
            str_caller = task.str_caller;
            str_caller_address = task.str_caller_address;
            str_contract_address = task.str_contract_address;
            str_contract_id = task.str_contract_id;
            str_method = task.str_method;
            str_args = task.str_args;
        }
    }
    intptr_t                statevalue;
    int                     num_limit;
    std::string             str_caller;
    std::string             str_caller_address;
    std::string             str_contract_address;
    std::string             str_contract_id;
    std::string             str_method;
    std::string             str_args;
    Code                    contract_code;
};

struct TransferTask : public TaskBase {
    TransferTask() {
        task_type = TRANSFER_TASK;
    };

    TransferTask(const TransferTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = TRANSFER_TASK;
            contract_code = task.contract_code;
            statevalue = task.statevalue;
            num_limit = task.num_limit;
            str_caller = task.str_caller;
            str_caller_address = task.str_caller_address;
            str_contract_address = task.str_contract_address;
            str_contract_id = task.str_contract_id;
            str_args = task.str_args;
        }
    }

    intptr_t                statevalue;
    int                     num_limit;
    std::string             str_caller;
    std::string             str_caller_address;
    std::string             str_contract_address;
    std::string             str_contract_id;
    std::string             str_args;
    Code                    contract_code;
};

struct DestroyTask : public TaskBase {
    DestroyTask() {
        task_type = DESTROY_TASK;
    };
    DestroyTask(const DestroyTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = DESTROY_TASK;
            contract_code = task.contract_code;
            statevalue = task.statevalue;
            num_limit = task.num_limit;
            str_caller = task.str_caller;
            str_caller_address = task.str_caller_address;
            str_contract_address = task.str_contract_address;
            str_contract_id = task.str_contract_id;
        }
    }

    intptr_t               statevalue;
    int                    num_limit;
    std::string            str_caller;
    std::string            str_caller_address;
    std::string            str_contract_address;
    std::string            str_contract_id;
    Code                   contract_code;
};

struct CompileScriptTask : public TaskBase {
    CompileScriptTask() {
        task_type = COMPILE_SCRIPT_TASK;
    };
    CompileScriptTask(const CompileScriptTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = COMPILE_SCRIPT_TASK;
            use_contract = task.use_contract;
            path_file_name = task.path_file_name;
            use_type_check = task.use_type_check;
            statevalue = task.statevalue;
        }
    }

    bool use_contract;
    std::string path_file_name;
    bool use_type_check;
    intptr_t  statevalue;
};

struct HandleEventsTask : public TaskBase {
    HandleEventsTask() {
        task_type = HANDLE_EVENTS_TASK;
    }

    HandleEventsTask(const HandleEventsTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = HANDLE_EVENTS_TASK;
            contract_id = task.contract_id;
            event_type = task.event_type;
            event_param = task.event_param;
            is_truncated = task.is_truncated;
            script_code = task.script_code;
        }
    }

    std::string contract_id;
    std::string event_type;
    std::string event_param;
    bool is_truncated;
    Code script_code;
};

struct CallContractOfflineTask : public TaskBase {
    CallContractOfflineTask() {
        task_type = CALL_OFFLINE_TASK;
    }

    CallContractOfflineTask(const CallContractOfflineTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_from = task.task_from;
            task_type = CALL_OFFLINE_TASK;
            statevalue = task.statevalue;
            num_limit = task.num_limit;
            str_caller = task.str_caller;
            str_caller_address = task.str_caller_address;
            str_contract_address = task.str_contract_address;
            str_contract_id = task.str_contract_id;
            str_method = task.str_method;
            str_args = task.str_args;
            contract_code = task.contract_code;
        }
    }

    intptr_t                statevalue;
    int                     num_limit;
    std::string             str_caller;
    std::string             str_caller_address;
    std::string             str_contract_address;
    std::string             str_contract_id;
    std::string             str_method;
    std::string             str_args;
    Code                    contract_code;
};

struct LuaRequestTask : public TaskBase {
    LuaRequestTask() {
        task_type = LUA_REQUEST_TASK;
        task_from = FROM_LUA_TO_CHAIN;
    }

    LuaRequestTask(const LuaRequestTask& task) {
        if (this != &task) {
            task_id = task.task_id;
            task_type = LUA_REQUEST_TASK;
            task_from = FROM_LUA_TO_CHAIN;
            method = task.method;
            params = task.params;
            statevalue = task.statevalue;
        }
    }

    LUA_REQUEST_METHOD     method;
    std::vector<std::vector<char>> params;
    intptr_t statevalue;
};

struct LuaRequestTaskResult : public TaskBase {
    LuaRequestTaskResult() {
        task_type = LUA_REQUEST_RESULT_TASK;
        task_from = FROM_LUA_TO_CHAIN;
    }

    LuaRequestTaskResult(const LuaRequestTaskResult& task) {
        if (this != &task) {
            task_type = LUA_REQUEST_RESULT_TASK;
            task_from = FROM_LUA_TO_CHAIN;
            method = task.method;
            params = task.params;
            ret = task.ret;
            err_num = task.err_num;
            task_id = task.task_id;
        }
    }
    LUA_REQUEST_METHOD     method;
    std::vector<std::vector<char>> params;
    int ret;
    int err_num;

};

FC_REFLECT_TYPENAME(LUA_TASK_FROM)
FC_REFLECT_ENUM(LUA_TASK_FROM,
                (FROM_CLI)
                (FROM_RPC)
                (FROM_LUA_TO_CHAIN)
               )

FC_REFLECT_TYPENAME(LUA_REQUEST_METHOD)

FC_REFLECT_ENUM(LUA_REQUEST_METHOD,
                (GET_STORED_CONTRACT_INFO_BY_ADDRESS)
                (GET_CONTRACT_ADDRESS_BY_NAME)
                (CHECK_CONTRACT_EXIST_BY_ADDRESS)
                (CHECK_CONTRACT_EXIST)
                (OPEN_CONTRACT)
                (OPEN_CONTRACT_BY_ADDRESS)
                (GET_STORAGE_VALUE_FROM_THINKYOUNG)
                (GET_CONTRACT_BALANCE_AMOUNT)
                (GET_TRANSACTION_FEE)
                (GET_CHAIN_NOW)
                (GET_CHAIN_RANDOM)
                (GET_TRANSACTION_ID)
                (GET_HEADER_BLOCK_NUM)
                (WAIT_FOR_FUTURE_RANDOM)
                (GET_WAITED)
                (COMMIT_STORAGE_CHANGES_TO_THINKYOUNG)
                (TRANSFER_FROM_CONTRACT_TO_ADDRESS)
                (TRANSFER_FROM_CONTRACT_TO_PUBLIC_ACCOUNT)
                (EMIT)
               )

FC_REFLECT_TYPENAME(LUA_TASK_TYPE)
FC_REFLECT_ENUM(LUA_TASK_TYPE,
                (COMPILE_TASK)
                (COMPILE_TASK_RESULT)
                (COMPILE_SCRIPT_TASK)
                (COMPILE_SCRIPT_RESULT)
                (REGISTER_TASK)
                (REGISTER_TASK_RESULT)
                (UPGRADE_TASK)
                (UPGRADE_TASK_RESULT)
                (CALL_TASK)
                (CALL_TASK_RESULT)
                (CALL_OFFLINE_TASK)
                (CALL_OFFLINE_TASK_RESULT)
                (TRANSFER_TASK)
                (TRANSFER_TASK_RESULT)
                (DESTROY_TASK)
                (DESTROY_TASK_RESULT)
                (HANDLE_EVENTS_TASK)
                (HANDLE_EVENTS_TASK_RESULT)
                (LUA_REQUEST_TASK)
                (LUA_REQUEST_RESULT_TASK)
                (HELLO_MSG)
               )

FC_REFLECT(TaskBase, (task_id)(task_type)(task_from))
FC_REFLECT_DERIVED(CompileTask, (TaskBase), (glua_path_file))

FC_REFLECT_DERIVED(CallTask, (TaskBase), (statevalue)(num_limit)
                   (str_caller)(str_caller_address)(str_contract_address)
                   (str_contract_id)(str_method)(str_args)(contract_code))

FC_REFLECT_DERIVED(RegisterTask, (TaskBase), (gpc_code)(statevalue)(num_limit)
                   (str_caller)(str_caller_address)(str_contract_address)
                   (str_contract_id)(contract_code))

FC_REFLECT_DERIVED(TransferTask, (TaskBase), (statevalue)(num_limit)
                   (str_caller)(str_caller_address)(str_contract_address)
                   (str_contract_id)(str_args)(contract_code))

FC_REFLECT_DERIVED(UpgradeTask, (TaskBase), (statevalue)(num_limit)
                   (str_caller)(str_caller_address)(str_contract_address)
                   (str_contract_id)(contract_code))

FC_REFLECT_DERIVED(DestroyTask, (TaskBase), (statevalue)(num_limit)
                   (str_caller)(str_caller_address)(str_contract_address)
                   (str_contract_id)(contract_code))

FC_REFLECT_DERIVED(CompileScriptTask, (TaskBase), (path_file_name)(use_contract)
                   (use_type_check)(statevalue))
FC_REFLECT_DERIVED(HandleEventsTask, (TaskBase), (contract_id)(event_type)
                   (event_param)(is_truncated)(script_code))
FC_REFLECT_DERIVED(CallContractOfflineTask, (TaskBase), (statevalue)(num_limit)
                   (str_caller)(str_caller_address)(str_contract_address)
                   (str_contract_id)(str_method)(str_args)(contract_code))

FC_REFLECT_DERIVED(LuaRequestTask, (TaskBase), (method)(params)(statevalue))
FC_REFLECT_DERIVED(LuaRequestTaskResult, (TaskBase), (method)(params)(ret)(err_num))

FC_REFLECT_DERIVED(TaskImplResult, (TaskBase), (error_code)(execute_count)(error_msg)(json_string))
FC_REFLECT_DERIVED(CompileTaskResult, (TaskImplResult), (gpc_path_file))
FC_REFLECT_DERIVED(RegisterTaskResult, (TaskImplResult))
FC_REFLECT_DERIVED(CallTaskResult, (TaskImplResult))
FC_REFLECT_DERIVED(TransferTaskResult, (TaskImplResult))
FC_REFLECT_DERIVED(UpgradeTaskResult, (TaskImplResult))
FC_REFLECT_DERIVED(DestroyTaskResult, (TaskImplResult))
FC_REFLECT_DERIVED(CompileScriptTaskResult, (TaskImplResult), (script_path_file))
FC_REFLECT_DERIVED(HandleEventsTaskResult, (TaskImplResult))
FC_REFLECT_DERIVED(CallContractOfflineTaskResult, (TaskImplResult))

#endif
