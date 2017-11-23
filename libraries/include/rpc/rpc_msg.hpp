/*
author: saiy
date: 2017.10.17
rpc message
*/

#ifndef _RPC_MSG_H_
#define _RPC_MSG_H_


#include <fc/array.hpp>
#include <fc/io/varint.hpp>
#include <fc/io/raw.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/reflect/variant.hpp>
#include <util/util.hpp>
#include <task/task.hpp>


enum SocketMode {
    ASYNC_MODE = 0,
    SYNC_MODE,
    MODE_COUNT
};

enum LuaRpcMessageTypeEnum {
    COMPILE_MESSAGE_TYPE = 0,
    COMPILE_RESULT_MESSAGE_TYPE,
    COMPILE_SCRIPT_MESSAGE_TPYE,
    COMPILE_SCRIPT_RESULT_MESSAGE_TPYE,
    CALL_MESSAGE_TYPE,
    CALL_RESULT_MESSAGE_TYPE,
    REGTISTER_MESSAGE_TYPE,
    REGTISTER_RESULT_MESSAGE_TYPE,
    UPGRADE_MESSAGE_TYPE,
    UPGRADE_RESULT_MESSAGE_TYPE,
    TRANSFER_MESSAGE_TYPE,
    TRANSFER_RESULT_MESSAGE_TYPE,
    DESTROY_MESSAGE_TYPE,
    DESTROY_RESULT_MESSAGE_TYPE,
    CALL_OFFLINE_MESSAGE_TYPE,
    CALL_OFFLINE_RESULT_MESSAGE_TYPE,
    HANDLE_EVENTS_MESSAGE_TYPE,
    HANDLE_EVENTS_RESULT_MESSAGE_TYPE,
    LUA_REQUEST_MESSAGE_TYPE,
    LUA_REQUEST_RESULT_MESSAGE_TYPE,
    HELLO_MESSAGE_TYPE,
    MESSAGE_COUNT
};

struct MessageHeader {
    uint32_t        size;//number of bytes in message, capped at MAX_MESSAGE_SIZE
    uint32_t        msg_type;
};

typedef fc::uint160_t MessageHashType;

/**
*  Abstracts the process of packing/unpacking a message for a
*  particular channel.
*/
struct Message : public MessageHeader {
    std::vector<char> data;
    
    Message() {}
    
    Message(Message&& m)
        :MessageHeader(m), data(std::move(m.data)) {}
        
    Message(const Message& m)
        :MessageHeader(m), data(m.data) {}
        
    /**
    *  Assumes that T::type specifies the message type
    */
    template<typename T>
    Message(const T& m) {
        msg_type = T::type;
        data = fc::raw::pack(m);
        size = (uint32_t)data.size();
    }
    
    fc::uint160_t id()const {
        return fc::ripemd160::hash(data.data(), (uint32_t)data.size());
    }
    
    /**
    *  Automatically checks the type and deserializes T in the
    *  opposite process from the constructor.
    */
    template<typename T>
    T as()const {
        try {
            FC_ASSERT(msg_type == T::type);
            T tmp;
            
            if (data.size()) {
                fc::datastream<const char*> ds(data.data(), data.size());
                fc::raw::unpack(ds, tmp);
                
            } else {
                // just to make sure that tmp shouldn't have any data
                fc::datastream<const char*> ds(nullptr, 0);
                fc::raw::unpack(ds, tmp);
            }
            
            return tmp;
        }
        
        FC_RETHROW_EXCEPTIONS(warn,
                              "error unpacking network message as a '${type}'  ${x} !=? ${msg_type}",
                              ("type", fc::get_typename<T>::name())
                              ("x", T::type)
                              ("msg_type", msg_type)
                             );
    }
};


//HELLO MSG
//hello msg, lvm send hello-msg to achain only, not receive hello-msg
struct HelloMsgRpc {
    static const LuaRpcMessageTypeEnum type;
    HelloMsg data;
    HelloMsgRpc() {}
    HelloMsgRpc(HelloMsg& para) :
        data(std::move(para))
    {}
};

struct HelloMsgResultRpc {
    static const LuaRpcMessageTypeEnum type;
    HelloMsgResult data;
    HelloMsgResultRpc() {}
    HelloMsgResultRpc(HelloMsgResult& para) :
        data(std::move(para))
    {}
};

//task:
struct CompileTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    CompileTask data;
    
    CompileTaskRpc() {}
    CompileTaskRpc(CompileTask& para) :
        data(std::move(para))
    {}
};

struct CallTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    CallTask data;
    
    CallTaskRpc() {}
    CallTaskRpc(CallTask& para) :
        data(std::move(para))
    {}
};

struct RegisterTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    RegisterTask data;
    
    RegisterTaskRpc() {}
    RegisterTaskRpc(RegisterTask& para) :
        data(std::move(para))
    {}
};

struct UpgradeTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    UpgradeTask data;
    
    UpgradeTaskRpc() {}
    UpgradeTaskRpc(UpgradeTask& para) :
        data(std::move(para))
    {}
};

struct TransferTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    TransferTask data;
    
    TransferTaskRpc() {}
    TransferTaskRpc(TransferTask& para) :
        data(std::move(para))
    {}
};

struct DestroyTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    DestroyTask data;
    
    DestroyTaskRpc() {}
    DestroyTaskRpc(DestroyTask& para) :
        data(std::move(para))
    {}
};

struct LuaRequestTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    LuaRequestTask data;
    
    LuaRequestTaskRpc() {}
    LuaRequestTaskRpc(LuaRequestTask& para) :
        data(std::move(para))
    {}
};

struct CompileScriptTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    CompileScriptTask data;
    
    CompileScriptTaskRpc() {}
    CompileScriptTaskRpc(CompileScriptTask& para) :
        data(std::move(para))
    {}
};

struct HandleEventsTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    HandleEventsTask data;
    
    HandleEventsTaskRpc() {}
    HandleEventsTaskRpc(HandleEventsTask& para) :
        data(std::move(para))
    {}
};

struct CallContractOfflineTaskRpc {
    static const LuaRpcMessageTypeEnum type;
    CallContractOfflineTask data;
    
    CallContractOfflineTaskRpc() {}
    CallContractOfflineTaskRpc(CallContractOfflineTask& para) :
        data(std::move(para))
    {}
};

//result:
struct CompileTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    CompileTaskResult data;
    
    CompileTaskResultRpc() {}
    CompileTaskResultRpc(CompileTaskResult& para) :
        data(std::move(para))
    {}
};

struct RegisterTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    RegisterTaskResult data;
    
    RegisterTaskResultRpc() {}
    RegisterTaskResultRpc(RegisterTaskResult& para) :
        data(std::move(para))
    {}
};

struct CallTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    CallTaskResult data;
    
    CallTaskResultRpc() {}
    CallTaskResultRpc(CallTaskResult& para) :
        data(std::move(para))
    {}
};

struct TransferTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    TransferTaskResult data;
    
    TransferTaskResultRpc() {}
    TransferTaskResultRpc(TransferTaskResult& para) :
        data(std::move(para))
    {}
};

struct UpgradeTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    UpgradeTaskResult data;
    
    UpgradeTaskResultRpc() {}
    UpgradeTaskResultRpc(UpgradeTaskResult& para) :
        data(std::move(para))
    {}
};

struct DestroyTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    DestroyTaskResult data;
    
    DestroyTaskResultRpc() {}
    DestroyTaskResultRpc(DestroyTaskResult& para) :
        data(std::move(para))
    {}
};

struct LuaRequestTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    LuaRequestTaskResult data;
    
    LuaRequestTaskResultRpc() {}
    LuaRequestTaskResultRpc(LuaRequestTaskResult& para) :
        data(std::move(para))
    {}
};

struct CompileScriptTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    CompileScriptTaskResult data;
    
    CompileScriptTaskResultRpc() {}
    CompileScriptTaskResultRpc(CompileScriptTaskResult& para) :
        data(std::move(para))
    {}
};

struct HandleEventsTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    HandleEventsTaskResult data;
    
    HandleEventsTaskResultRpc() {}
    HandleEventsTaskResultRpc(HandleEventsTaskResult& para) :
        data(std::move(para))
    {}
};

struct CallContractOfflineTaskResultRpc {
    static const LuaRpcMessageTypeEnum type;
    CallContractOfflineTaskResult data;
    
    CallContractOfflineTaskResultRpc() {}
    CallContractOfflineTaskResultRpc(CallContractOfflineTaskResult& para) :
        data(std::move(para))
    {}
};

FC_REFLECT_ENUM(LuaRpcMessageTypeEnum,
                (COMPILE_MESSAGE_TYPE)
                (COMPILE_RESULT_MESSAGE_TYPE)
                (COMPILE_SCRIPT_MESSAGE_TPYE)
                (COMPILE_SCRIPT_RESULT_MESSAGE_TPYE)
                (CALL_MESSAGE_TYPE)
                (CALL_RESULT_MESSAGE_TYPE)
                (REGTISTER_MESSAGE_TYPE)
                (REGTISTER_RESULT_MESSAGE_TYPE)
                (UPGRADE_MESSAGE_TYPE)
                (UPGRADE_RESULT_MESSAGE_TYPE)
                (TRANSFER_MESSAGE_TYPE)
                (TRANSFER_RESULT_MESSAGE_TYPE)
                (DESTROY_MESSAGE_TYPE)
                (DESTROY_RESULT_MESSAGE_TYPE)
                (CALL_OFFLINE_MESSAGE_TYPE)
                (CALL_OFFLINE_RESULT_MESSAGE_TYPE)
                (HANDLE_EVENTS_MESSAGE_TYPE)
                (HANDLE_EVENTS_RESULT_MESSAGE_TYPE)
                (LUA_REQUEST_MESSAGE_TYPE)
                (LUA_REQUEST_RESULT_MESSAGE_TYPE)
                (HELLO_MESSAGE_TYPE)
               )

FC_REFLECT(MessageHeader, (size)(msg_type))
FC_REFLECT_DERIVED(Message, (MessageHeader), (data))

FC_REFLECT(CompileTaskRpc, (data))
FC_REFLECT(CallTaskRpc, (data))
FC_REFLECT(RegisterTaskRpc, (data))
FC_REFLECT(TransferTaskRpc, (data))
FC_REFLECT(UpgradeTaskRpc, (data))
FC_REFLECT(DestroyTaskRpc, (data))
FC_REFLECT(LuaRequestTaskRpc, (data))
FC_REFLECT(CompileScriptTaskRpc, (data))
FC_REFLECT(HandleEventsTaskRpc, (data))
FC_REFLECT(CallContractOfflineTaskRpc, (data))

//result
FC_REFLECT(CompileTaskResultRpc, (data))
FC_REFLECT(RegisterTaskResultRpc, (data))
FC_REFLECT(CallTaskResultRpc, (data))
FC_REFLECT(TransferTaskResultRpc, (data))
FC_REFLECT(UpgradeTaskResultRpc, (data))
FC_REFLECT(DestroyTaskResultRpc, (data))
FC_REFLECT(LuaRequestTaskResultRpc, (data))
FC_REFLECT(CompileScriptTaskResultRpc, (data))
FC_REFLECT(HandleEventsTaskResultRpc, (data))
FC_REFLECT(CallContractOfflineTaskResultRpc, (data))

//hello msg
FC_REFLECT(HelloMsgRpc, (data))
FC_REFLECT(HelloMsgResultRpc, (data))

#endif