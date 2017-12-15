#include <rpc/rpc_task_handler.hpp>
#include <rpc/rpc_mgr.hpp>
#include <rpc/rpc_msg.hpp>
#include <base/exceptions.hpp>
#include <iostream>
#include <mutex>

//task
const LuaRpcMessageTypeEnum CompileTaskRpc::type = LuaRpcMessageTypeEnum::COMPILE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallTaskRpc::type = LuaRpcMessageTypeEnum::CALL_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum RegisterTaskRpc::type = LuaRpcMessageTypeEnum::REGTISTER_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum UpgradeTaskRpc::type = LuaRpcMessageTypeEnum::UPGRADE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum DestroyTaskRpc::type = LuaRpcMessageTypeEnum::DESTROY_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum TransferTaskRpc::type = LuaRpcMessageTypeEnum::TRANSFER_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum LuaRequestTaskRpc::type = LuaRpcMessageTypeEnum::LUA_REQUEST_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CompileScriptTaskRpc::type = LuaRpcMessageTypeEnum::COMPILE_SCRIPT_MESSAGE_TPYE;
const LuaRpcMessageTypeEnum HandleEventsTaskRpc::type = LuaRpcMessageTypeEnum::HANDLE_EVENTS_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallContractOfflineTaskRpc::type = LuaRpcMessageTypeEnum::CALL_OFFLINE_MESSAGE_TYPE;

//result
const LuaRpcMessageTypeEnum CompileTaskResultRpc::type = LuaRpcMessageTypeEnum::COMPILE_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum RegisterTaskResultRpc::type = LuaRpcMessageTypeEnum::REGTISTER_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallTaskResultRpc::type = LuaRpcMessageTypeEnum::CALL_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum UpgradeTaskResultRpc::type = LuaRpcMessageTypeEnum::UPGRADE_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum DestroyTaskResultRpc::type = LuaRpcMessageTypeEnum::DESTROY_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum TransferTaskResultRpc::type = LuaRpcMessageTypeEnum::TRANSFER_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum LuaRequestTaskResultRpc::type = LuaRpcMessageTypeEnum::LUA_REQUEST_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CompileScriptTaskResultRpc::type = LuaRpcMessageTypeEnum::COMPILE_SCRIPT_RESULT_MESSAGE_TPYE;
const LuaRpcMessageTypeEnum HandleEventsTaskResultRpc::type = LuaRpcMessageTypeEnum::HANDLE_EVENTS_RESULT_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallContractOfflineTaskResultRpc::type = LuaRpcMessageTypeEnum::CALL_OFFLINE_RESULT_MESSAGE_TYPE;

//hello msg
const LuaRpcMessageTypeEnum HelloMsgRpc::type = LuaRpcMessageTypeEnum::HELLO_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum HelloMsgResultRpc::type = LuaRpcMessageTypeEnum::HELLO_MESSAGE_TYPE;

RpcTaskHandler::RpcTaskHandler(RpcMgr* rpcMgrPtr) {
    _rpc_mgr_ptr = rpcMgrPtr;
}

RpcTaskHandler::~RpcTaskHandler() {
}

void RpcTaskHandler::string_to_msg(const std::string& str_msg, Message& msg) {
    uint64_t data_size = str_msg.length() - sizeof(MessageHeader);
    /*msgheader*/
    memcpy((char*)&msg, str_msg.c_str(), sizeof(MessageHeader));
    /*data*/
    msg.data.resize(data_size);
    memcpy((char*)&msg.data[0], str_msg.c_str() + sizeof(MessageHeader), data_size);
    msg.data.resize(msg.size);
}

TaskBase* RpcTaskHandler::parse_to_task(const std::string& task,
                                        fc::buffered_istream* argument_stream) {
    //parse the msg from chain
    //include two parts: part 1: chain call contract operation; part 2: chain response to lvm LUA_REQUEST
    Message m;
    string_to_msg(task, m);
    
    try {
        switch (m.msg_type) {
            case HELLO_MESSAGE_TYPE: {
                HelloMsgRpc hello_msg(m.as<HelloMsgRpc>());
                HelloMsg* hello_ptr = new HelloMsg();
                memcpy(hello_ptr, &hello_msg.data, sizeof(hello_msg.data));
                return hello_ptr;
            }
            
            case COMPILE_MESSAGE_TYPE: {
                CompileTaskRpc compile_task(m.as<CompileTaskRpc>());
                CompileTask* compile_ptr = new CompileTask(compile_task.data);
                FC_ASSERT(compile_ptr->task_type == compile_task.data.task_type, "", \
                          ("CompileTask::task_type", compile_ptr->task_type) \
                          ("CompileTaskRpc::task_type", compile_task.data.task_type));
                return compile_ptr;
            }
            
            case CALL_MESSAGE_TYPE: {
                CallTaskRpc call_task(m.as<CallTaskRpc>());
                CallTask* call_ptr = new CallTask(call_task.data);
                FC_ASSERT(call_ptr->task_type == call_task.data.task_type, "", \
                          ("CallTask::task_type", call_ptr->task_type) \
                          ("CallTaskRpc::task_type", call_task.data.task_type));
                return call_ptr;
            }
            
            case REGTISTER_MESSAGE_TYPE: {
                RegisterTaskRpc register_task(m.as<RegisterTaskRpc>());
                RegisterTask* register_ptr = new RegisterTask(register_task.data);
                FC_ASSERT(register_ptr->task_type == register_task.data.task_type, "", \
                          ("RegisterTask::task_type", register_ptr->task_type) \
                          ("RegisterTaskRpc::task_type", register_task.data.task_type));
                return register_ptr;
            }
            
            case UPGRADE_MESSAGE_TYPE: {
                UpgradeTaskRpc upgrade_task(m.as<UpgradeTaskRpc>());
                UpgradeTask* upgrade_ptr = new UpgradeTask(upgrade_task.data);
                FC_ASSERT(upgrade_ptr->task_type == upgrade_task.data.task_type, "", \
                          ("UpgradeTask::task_type", upgrade_ptr->task_type) \
                          ("UpgradeTaskRpc::task_type", upgrade_task.data.task_type));
                return upgrade_ptr;
            }
            
            case TRANSFER_MESSAGE_TYPE: {
                TransferTaskRpc transfer_task(m.as<TransferTaskRpc>());
                TransferTask* transfer_ptr = new TransferTask(transfer_task.data);
                FC_ASSERT(transfer_ptr->task_type == transfer_task.data.task_type, "", \
                          ("TransferTask::task_type", transfer_ptr->task_type) \
                          ("TransferTaskRpc::task_type", transfer_task.data.task_type));
                return transfer_ptr;
            }
            
            case DESTROY_MESSAGE_TYPE: {
                DestroyTaskRpc destroy_task(m.as<DestroyTaskRpc>());
                DestroyTask* destroy_ptr = new DestroyTask(destroy_task.data);
                FC_ASSERT(destroy_ptr->task_type == destroy_task.data.task_type, "", \
                          ("DestroyTask::task_type", destroy_ptr->task_type) \
                          ("DestroyTaskRpc::task_type", destroy_task.data.task_type));
                return destroy_ptr;
            }
            
            case COMPILE_SCRIPT_MESSAGE_TPYE: {
                CompileScriptTaskRpc compile_script_task(m.as<CompileScriptTaskRpc>());
                CompileScriptTask* compile_script_ptr = new CompileScriptTask(compile_script_task.data);
                FC_ASSERT(compile_script_ptr->task_type == compile_script_task.data.task_type, "", \
                          ("CompileScriptTask::task_type", compile_script_ptr->task_type) \
                          ("CompileScriptTaskRpc::task_type", compile_script_task.data.task_type));
                return compile_script_ptr;
            }
            
            case HANDLE_EVENTS_MESSAGE_TYPE: {
                HandleEventsTaskRpc handle_events_task(m.as<HandleEventsTaskRpc>());
                HandleEventsTask* handle_events_ptr = new HandleEventsTask(handle_events_task.data);
                FC_ASSERT(handle_events_ptr->task_type == handle_events_task.data.task_type, "", \
                          ("HandleEventsTask::task_type", handle_events_ptr->task_type) \
                          ("HandleEventsTaskRpc::task_type", handle_events_task.data.task_type));
                return handle_events_ptr;
            }
            
            case CALL_OFFLINE_MESSAGE_TYPE: {
                CallContractOfflineTaskRpc call_contract_offline_task(m.as<CallContractOfflineTaskRpc>());
                CallContractOfflineTask* call_contract_offline_ptr = new CallContractOfflineTask(call_contract_offline_task.data);
                FC_ASSERT(call_contract_offline_ptr->task_type == call_contract_offline_task.data.task_type, "", \
                          ("CallContractOfflineTask::task_type", call_contract_offline_ptr->task_type) \
                          ("CallContractOfflineTaskRpc::task_type", call_contract_offline_task.data.task_type));
                return call_contract_offline_ptr;
            }
            
            case LUA_REQUEST_RESULT_MESSAGE_TYPE: {
                LuaRequestTaskResultRpc lua_request_result_task(m.as<LuaRequestTaskResultRpc>());
                LuaRequestTaskResult* lua_request_result_ptr = new LuaRequestTaskResult(lua_request_result_task.data);
                FC_ASSERT(lua_request_result_ptr->task_type == lua_request_result_task.data.task_type, "", \
                          ("LuaRequestTaskResult::task_type", lua_request_result_ptr->task_type) \
                          ("LuaRequestTaskResultRpc::task_type", lua_request_result_task.data.task_type));
                return lua_request_result_ptr;
            }
            
            default:
                FC_THROW_EXCEPTION(lvm::global_exception::rpc_msg_error, \
                                   "the msg_type of rpc request error " \
                                   "$ {msg_type}", ("msg_type", m.msg_type));
                return nullptr;
        }
        
    } catch (const lvm::global_exception::rpc_msg_error& e) {
        //TODO
    }
    
    return nullptr;
}

void RpcTaskHandler::task_finished(TaskImplResult* result) {
    //response: lvm to chain
    //the circle: chain start request,lvm receive and response the msg, the msg type is xxxx_RESULT_MESSAGE_xxxx
    FC_ASSERT(result != NULL);
    //the msg type is xxxx_RESULT_MESSAGE_xxxx
    Message msg(generate_message(result));
    post_result(msg);
}

LuaRequestTaskResult RpcTaskHandler::lua_request(LuaRequestTask& request_task) {
    LuaRequestTaskResult* result_p = nullptr;
    FC_ASSERT(_rpc_mgr_ptr != NULL);
    _lua_request_promise_ptr = fc::promise<void*>::ptr(new fc::promise<void*>("lua_request_promise"));
    std::shared_ptr<LuaRequestTaskResult> result_ptr;
    post_message(request_task);
    result_p = (LuaRequestTaskResult*)(void *)_lua_request_promise_ptr->wait();
    result_ptr.reset(result_p);
    _lua_request_promise_ptr.reset();
    FC_ASSERT(result_ptr && result_ptr->task_type == LUA_REQUEST_RESULT_TASK);
    LuaRequestTaskResult lua_request_result = *result_ptr;
    return lua_request_result;
}

void RpcTaskHandler::post_result(Message& msg) {
    //lvm response to chain
    FC_ASSERT(_rpc_mgr_ptr != NULL);
    return _rpc_mgr_ptr->post_message(msg);
}

void RpcTaskHandler::post_message(LuaRequestTask& lua_task) {
    //lvm do request to chain
    FC_ASSERT(_rpc_mgr_ptr != NULL);
    LuaRequestTaskRpc lua_msg(lua_task);
    Message msg(lua_msg);
    store_request(lua_task);
    _rpc_mgr_ptr->post_message(msg);
}

void RpcTaskHandler::store_request(LuaRequestTask& task) {
    std::lock_guard<std::mutex> auto_guard(_task_mutex);
    _tasks.push_back(task);
}

void RpcTaskHandler::set_value(const std::string& result) {
    Message m;
    LuaRequestTaskResult* p_result = nullptr;
    string_to_msg(result, m);
    LuaRequestTaskResultRpc lua_request_result_task(m.as<LuaRequestTaskResultRpc>());
    std::lock_guard<std::mutex> auto_guard(_task_mutex);
    auto iter = std::find_if(begin(_tasks), end(_tasks), [=](LuaRequestTask lua_request_task) {
        return (lua_request_task.task_id == lua_request_result_task.data.task_id);
    });
    FC_ASSERT(_lua_request_promise_ptr);
    
    if (iter != _tasks.end()) {
        if (_lua_request_promise_ptr && (!_lua_request_promise_ptr->canceled())) {
            p_result = new LuaRequestTaskResult(lua_request_result_task.data);
            _lua_request_promise_ptr->set_value(p_result);
        }
        
        _tasks.erase(iter);
    }
}

Message RpcTaskHandler::generate_message(TaskImplResult* task_ptr) {
    FC_ASSERT(task_ptr != NULL);
    FC_ASSERT(task_ptr->task_type == HELLO_MSG || task_ptr->task_type == COMPILE_TASK
              || task_ptr->task_type == REGISTER_TASK ||task_ptr->task_type == UPGRADE_TASK
              || task_ptr->task_type == CALL_TASK ||task_ptr->task_type == TRANSFER_TASK
              || task_ptr->task_type == DESTROY_TASK || task_ptr->task_type == COMPILE_SCRIPT_TASK
              || task_ptr->task_type == HANDLE_EVENTS_TASK || task_ptr->task_type == CALL_OFFLINE_TASK);
    return task_ptr->get_rpc_message();
}
