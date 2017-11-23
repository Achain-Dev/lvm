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