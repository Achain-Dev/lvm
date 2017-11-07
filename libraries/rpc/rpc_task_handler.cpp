
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

//result
const LuaRpcMessageTypeEnum CompileTaskResultRpc::type = LuaRpcMessageTypeEnum::COMPILE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum RegisterTaskResultRpc::type = LuaRpcMessageTypeEnum::REGTISTER_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum CallTaskResultRpc::type = LuaRpcMessageTypeEnum::CALL_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum UpgradeTaskResultRpc::type = LuaRpcMessageTypeEnum::UPGRADE_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum DestroyTaskResultRpc::type = LuaRpcMessageTypeEnum::DESTROY_MESSAGE_TYPE;
const LuaRpcMessageTypeEnum TransferTaskResultRpc::type = LuaRpcMessageTypeEnum::TRANSFER_MESSAGE_TYPE;


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
            case COMPILE_MESSAGE_TYPE: {
                CompileTaskRpc compile_task(m.as<CompileTaskRpc>());
                CompileTask* compile_ptr = new CompileTask();
                FC_ASSERT(compile_ptr->task_type == compile_task.data.task_type, "", \
                          ("CompileTask::task_type", compile_ptr->task_type) \
                          ("CompileTaskRpc::task_type", compile_task.data.task_type));
                memcpy(compile_ptr, &compile_task.data, sizeof(compile_task.data));
                return compile_ptr;
            }
            
            case CALL_MESSAGE_TYPE: {
                CallTaskRpc call_task(m.as<CallTaskRpc>());
                CallTask* call_ptr = new CallTask();
                FC_ASSERT(call_ptr->task_type == call_task.data.task_type, "", \
                          ("CallTask::task_type", call_ptr->task_type) \
                          ("CallTaskRpc::task_type", call_task.data.task_type));
                memcpy(call_ptr, &call_task.data, sizeof(call_task.data));
                return call_ptr;
            }
            
            case REGTISTER_MESSAGE_TYPE: {
                RegisterTaskRpc register_task(m.as<RegisterTaskRpc>());
                RegisterTask* register_ptr = new RegisterTask();
                FC_ASSERT(register_ptr->task_type == register_task.data.task_type, "", \
                          ("CallTask::task_type", register_ptr->task_type) \
                          ("CallTaskRpc::task_type", register_task.data.task_type));
                memcpy(register_ptr, &register_task.data, sizeof(register_task.data));
                return register_ptr;
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
    
    //TODO
    return nullptr;
}

void RpcTaskHandler::task_finished(TaskImplResult* result) {
    FC_ASSERT(result != NULL);
    FC_ASSERT(result->task_from == FROM_RPC);
    Message msg(generate_message(result));
    send_message(msg);
}


void RpcTaskHandler::send_message(Message& msg) {
    FC_ASSERT(_rpc_mgr_ptr != NULL);
    return _rpc_mgr_ptr->send_message(msg);
}

Message RpcTaskHandler::generate_message(TaskImplResult* task_ptr) {
    FC_ASSERT(task_ptr != NULL);
    FC_ASSERT(task_ptr->task_type == COMPILE_TASK || task_ptr->task_type == REGISTER_TASK ||
              task_ptr->task_type == UPGRADE_TASK || task_ptr->task_type == CALL_TASK ||
              task_ptr->task_type == TRANSFER_TASK || task_ptr->task_type == DESTROY_TASK);
    return task_ptr->get_rpc_message();
}