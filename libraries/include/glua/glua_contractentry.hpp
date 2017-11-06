#ifndef _GLUA_CONTRACTENTRY_H
#define _GLUA_CONTRACTENTRY_H

#include <glua/lua_api.h>

#include <boost/uuid/sha1.hpp>

#include <fc/filesystem.hpp>
#include <fc/io/enum_type.hpp>
#include <fc/optional.hpp>
#include <fc/reflect/reflect.hpp>

#include <set>
#include <string>
#include <vector>

#define PRINTABLE_CHAR(chr) \
if (chr >= 0 && chr <= 9)  \
    chr = chr + '0'; \
else \
    chr = chr + 'a' - 10;

enum ContractApiType {
    chain = 1,
    offline = 2,
    event = 3
};

enum ContractState {
    valid = 1,
    deleted = 2
};

enum ContractLevel {
    temp = 1,
    forever = 2
};

enum OperationTypeEnum {
    null_op_type = 0,
    
    // balances
    withdraw_op_type = 1,
    deposit_op_type = 2,
    
    // accounts
    register_account_op_type = 3,
    update_account_op_type = 4,
    withdraw_pay_op_type = 5,
    
    // assets
    create_asset_op_type = 6,
    update_asset_op_type = 7,
    issue_asset_op_type = 8,
    
    // reserved
    // reserved_op_1_type         = 10, // Skip; see below
    reserved_op_2_type = 11,
    reserved_op_3_type = 17,
    define_slate_op_type = 18,
    
    // reserved
    reserved_op_4_type = 21,
    reserved_op_5_type = 22,
    release_escrow_op_type = 23,
    update_signing_key_op_type = 24,
    update_balance_vote_op_type = 27,
    
    // assets
    update_asset_ext_op_type = 30,
    //memo
    imessage_memo_op_type = 66,
    
    contract_info_op_type = 68,
    
    register_contract_op_type = 70,
    upgrade_contract_op_type = 71,
    destroy_contract_op_type = 72,
    call_contract_op_type = 73,
    transfer_contract_op_type = 74,
    // contract
    withdraw_contract_op_type = 80,
    deposit_contract_op_type = 82,
    
    // balances withdraw
    balances_withdraw_op_type = 88,
    
    transaction_op_type = 90,
    storage_op_type = 91,
    
    // event
    event_op_type = 100,
    
    // on functions in contracts
    on_destroy_op_type = 108,
    on_upgrade_op_type = 109,
    
    // contract call success
    on_call_success_op_type = 110
};

typedef unsigned char ContractChar;

static std::string to_printable_hex(unsigned char chr) {
    unsigned char high = chr >> 4;
    unsigned char low = chr & 0x0F;
    char tmp[16];
    PRINTABLE_CHAR(high);
    PRINTABLE_CHAR(low);
    snprintf(tmp, sizeof(tmp), "%c%c", high, low);
    return std::string(tmp);
}

//the code-related detail of contract
struct Code {
    std::set<std::string> abi;
    std::set<std::string> offline_abi;
    std::set<std::string> events;
    std::map<std::string, fc::enum_type<fc::unsigned_int, lvm::blockchain::StorageValueTypes>> storage_properties;
    std::vector<ContractChar> byte_code;
    std::string code_hash;
    Code(const fc::path&);
    Code();
    void SetApis(char* module_apis[], int count, int api_type);
    bool valid() const;
    std::string GetHash() const;
};

FC_REFLECT(Code,
           (abi)
           (offline_abi)
           (events)
           (storage_properties)
           (byte_code)
           (code_hash)
          )


#endif