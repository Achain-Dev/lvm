#ifndef  _GLUA_CONTRACTOPERATIONS_H_
#define  _GLUA_CONTRACTOPERATIONS_H_

#include <base/misc.hpp>
#include <glua/glua_contractentry.hpp>
#include <glua/lua_lib.h>

#include <fc/reflect/reflect.hpp>

#include <stdint.h>
#include <string>


struct ContractOperation {
    virtual  void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const = 0;
};

struct RegisterContractOperation : ContractOperation {
    RegisterContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct UpgradeContractOperation: ContractOperation {
    UpgradeContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct DestroyContractOperation: ContractOperation {
    DestroyContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct CallContractOperation : ContractOperation  {
    CallContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct TransferContractOperation  : ContractOperation {
    TransferContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct CompileContractOperation : ContractOperation {
    CompileContractOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct CompileScriptOperation : ContractOperation {
    CompileScriptOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct HandleEventsOperation : ContractOperation {
    HandleEventsOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct CallContractOfflineOperation : ContractOperation {
    CallContractOfflineOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

struct HelloMsgOperation : ContractOperation {
    HelloMsgOperation() {}
    virtual void evaluate(TaskAndCallback& _inst_taskandcallback, TaskImplResult** result) const;
};

ContractOperation* BuildContractOperation(TaskBase* task);

static void setGluaStateScopeValue(lvm::lua::lib::GluaStateScope& scope,
                                   const std::string& str_caller,
                                   const std::string& str_caller_addr,
                                   const GluaStateValue& statevalue,
                                   const size_t limit_num
                                  );

static void store_contractinfo_in_chain(lvm::lua::lib::GluaStateScope& scope,
                                        const std::string& str_contract_address,
                                        const std::string& str_contract_id,
                                        Code& _code);
#endif
