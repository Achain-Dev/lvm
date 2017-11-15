#include <rpc/rpc_task_handler.hpp>
#include <rpc/rpc_mgr.hpp>
#include <rpc/rpc_msg.hpp>
#include <base/exceptions.hpp>
#include <iostream>

//task
const LuaRpcMessageTypeEnum CompileTaskRpc::type = LuaRpcMessageTypeEnum::COMPILE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallTaskRpc::type = LuaRpcMessageTypeEnum::CALL_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum RegisterTaskRpc::type = LuaRpcMessageTypeEnum::REGTISTER_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum UpgradeTaskRpc::type = LuaRpcMessageTypeEnum::UPGRADE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum DestroyTaskRpc::type = LuaRpcMessageTypeEnum::DESTROY_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum TransferTaskRpc::type = LuaRpcMessageTypeEnum::TRANSFER_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum LuaRequestTaskRpc::type = LuaRpcMessageTypeEnum::LUA_REQUEST_MESSAGE_TYPE;

//result
const LuaRpcMessageTypeEnum CompileTaskResultRpc::type = LuaRpcMessageTypeEnum::COMPILE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum RegisterTaskResultRpc::type = LuaRpcMessageTypeEnum::REGTISTER_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallTaskResultRpc::type = LuaRpcMessageTypeEnum::CALL_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum UpgradeTaskResultRpc::type = LuaRpcMessageTypeEnum::UPGRADE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum DestroyTaskResultRpc::type = LuaRpcMessageTypeEnum::DESTROY_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum TransferTaskResultRpc::type = LuaRpcMessageTypeEnum::TRANSFER_MESSAGE_TYPE;

//hello msg
const LuaRpcMessageTypeEnum HelloMsgRpc::type = LuaRpcMessageTypeEnum::HELLO_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum HelloMsgResultRpc::type = LuaRpcMessageTypeEnum::HELLO_MESSAGE_TYPE;



RpcTaskHandler::RpcTaskHandler(RpcMgr* rpcMgrPtr) {
    _rpc_mgr_ptr = rpcMgrPtr;
}

RpcTaskHandler::~RpcTaskHandler() {
}


TaskBase* RpcTaskHandler::parse_to_task(const std::string& task,
                                        fc::buffered_istream* argument_stream) {
    Message m;
    uint64_t data_size = task.length() - sizeof(MessageHeader);
    /*msgheader*/
    memcpy((char*)&m, task.c_str(), sizeof(MessageHeader));
    /*data*/
    m.data.resize(data_size);
    memcpy((char*)&m.data[0], task.c_str() + sizeof(MessageHeader), data_size);
    m.data.resize(m.size);
    
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
                          ("CallTask::task_type", register_ptr->task_type) \
                          ("CallTaskRpc::task_type", register_task.data.task_type));
                return register_ptr;
            }
            
            case UPGRADE_MESSAGE_TYPE: {
                UpgradeTaskRpc upgrade_task(m.as<UpgradeTaskRpc>());
                UpgradeTask* upgrade_ptr = new UpgradeTask(upgrade_task.data);
                FC_ASSERT(upgrade_ptr->task_type == upgrade_task.data.task_type, "", \
                          ("CompileTask::task_type", upgrade_ptr->task_type) \
                          ("CompileTaskRpc::task_type", upgrade_task.data.task_type));
                return upgrade_ptr;
            }
            
            case TRANSFER_MESSAGE_TYPE: {
                TransferTaskRpc transfer_task(m.as<TransferTaskRpc>());
                TransferTask* transfer_ptr = new TransferTask(transfer_task.data);
                FC_ASSERT(transfer_ptr->task_type == transfer_task.data.task_type, "", \
                          ("CompileTask::task_type", transfer_ptr->task_type) \
                          ("CompileTaskRpc::task_type", transfer_task.data.task_type));
                return transfer_ptr;
            }
            
            case DESTROY_MESSAGE_TYPE: {
                DestroyTaskRpc destroy_task(m.as<DestroyTaskRpc>());
                DestroyTask* destroy_ptr = new DestroyTask(destroy_task.data);
                FC_ASSERT(destroy_ptr->task_type == destroy_task.data.task_type, "", \
                          ("CompileTask::task_type", destroy_ptr->task_type) \
                          ("CompileTaskRpc::task_type", destroy_task.data.task_type));
                return destroy_ptr;
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
    FC_ASSERT(result != NULL);
    FC_ASSERT(result->task_from == FROM_RPC);
    Message msg(generate_message(result));
    post_message(msg);
}

void RpcTaskHandler::lua_request(LuaRequestTask& request_task,
                                 std::string& response_data) {
    FC_ASSERT(_rpc_mgr_ptr != NULL);
    _rpc_mgr_ptr->send_message(&request_task, response_data);
}


void RpcTaskHandler::post_message(Message& msg) {
    FC_ASSERT(_rpc_mgr_ptr != NULL);
    return _rpc_mgr_ptr->post_message(msg);
}

Message RpcTaskHandler::generate_message(TaskImplResult* task_ptr) {
    FC_ASSERT(task_ptr != NULL);
    FC_ASSERT(task_ptr->task_type == HELLO_MSG || task_ptr->task_type == COMPILE_TASK
              || task_ptr->task_type == REGISTER_TASK ||task_ptr->task_type == UPGRADE_TASK
              || task_ptr->task_type == CALL_TASK ||task_ptr->task_type == TRANSFER_TASK
              || task_ptr->task_type == DESTROY_TASK);
    return task_ptr->get_rpc_message();
}
