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

#define MAX_MESSAGE_SIZE   (512 * 10000 * 5)


enum LuaRpcMessageTypeEnum {
    COMPILE_MESSAGE_TYPE = 0,
    CALL_MESSAGE_TYPE,
    REGTISTER_MESSAGE_TYPE,
    UPGRADE_MESSAGE_TYPE,
    TRANSFER_MESSAGE_TYPE,
    DESTROY_MESSAGE_TYPE,
    MESSAGE_COUNT
};

struct MessageHeader {
    uint32_t        size;//number of bytes in message, capped at MAX_MESSAGE_SIZE
    uint32_t        msg_type;
    uint32_t        msg_id;// msg from chain has a unique id,lvm response the msg will use this id geted from chain msg.
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
        msg_id = 0;     //init to 0
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


FC_REFLECT_ENUM(LuaRpcMessageTypeEnum, (COMPILE_MESSAGE_TYPE)(CALL_MESSAGE_TYPE)(REGTISTER_MESSAGE_TYPE))
FC_REFLECT(MessageHeader, (size)(msg_id)(msg_type)(from))
FC_REFLECT_DERIVED(Message, (MessageHeader), (data))

FC_REFLECT(CompileTaskRpc, (data))
FC_REFLECT(CallTaskRpc, (data))
FC_REFLECT(RegisterTaskRpc, (data))
FC_REFLECT(TransferTaskRpc, (data))
FC_REFLECT(UpgradeTaskRpc, (data))
FC_REFLECT(DestroyTaskRpc, (data))

//result
FC_REFLECT(CompileTaskResultRpc, (data))
FC_REFLECT(RegisterTaskResultRpc, (data))
FC_REFLECT(CallTaskResultRpc, (data))
FC_REFLECT(TransferTaskResultRpc, (data))
FC_REFLECT(UpgradeTaskResultRpc, (data))
FC_REFLECT(DestroyTaskResultRpc, (data))



#endif