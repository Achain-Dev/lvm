#ifndef     _GLUA_CONTRACTOPERATIONS_H
#define    _GLUA_CONTRACTOPERATIONS_H

#include <glua/glua_contractentry.hpp>
#include <glua/thinkyoung_lua_lib.h>

#include <base/misc.hpp>
#include <fc/reflect/reflect.hpp>
#include <stdint.h>
#include <string>

struct ContractOperation {
    virtual  void evaluate(TaskAndCallback& _inst_taskandcallback) const=0;
};

struct RegisterContractOperation : ContractOperation {
    static const OperationTypeEnum type;
    RegisterContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback) const;
};

struct UpgradeContractOperation: ContractOperation {
    static const OperationTypeEnum type;
    
    UpgradeContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback) const;
};

struct DestroyContractOperation: ContractOperation {
    static const OperationTypeEnum type;
    DestroyContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback) const;
};


struct CallContractOperation : ContractOperation  {
    static const OperationTypeEnum type;
    CallContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback) const;
};

struct TransferContractOperation  : ContractOperation {
    static const OperationTypeEnum type;
    TransferContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback) const;
};

static void setGluaStateScopeValue(thinkyoung::lua::lib::GluaStateScope& scope,
                                   const std::string& str_caller,
                                   const std::string& str_caller_addr,
                                   const GluaStateValue& statevalue,
                                   const size_t limit_num
                                  );

#endif