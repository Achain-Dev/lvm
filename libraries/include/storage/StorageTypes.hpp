#pragma once

#include <fc/io/enum_type.hpp>
#include <fc/io/raw.hpp>

#include <glua/luaconf.h>
#include <glua/lua_api.h>
#include <glua/lua_lib.h>

namespace lvm {
    namespace blockchain {
    
        extern std::map <StorageValueTypes, std::string> storage_type_map;
        
        struct StorageDataType {
            fc::enum_type<uint8_t, StorageValueTypes>  storage_type;
            std::vector<char>                           storage_data;
            
            StorageDataType() :storage_type(storage_value_null) {}
            
            template<typename StorageType>
            StorageDataType(const StorageType& t) {
                storage_type = StorageType::type;
                storage_data = fc::raw::pack(t);
            }
            
            template<typename StorageType>
            StorageType as()const {
                FC_ASSERT(storage_type == StorageType::type, "", ("type", storage_type)("StorageType", StorageType::type));
                return fc::raw::unpack<StorageType>(storage_data);
            }
            
            static StorageDataType get_storage_data_from_lua_storage(const GluaStorageValue& lua_storage);
            static GluaStorageValue create_lua_storage_from_storage_data(lua_State *L, const StorageDataType& storage_data);
            
            inline static bool is_table_type(StorageValueTypes type) {
                return (type >= StorageValueTypes::storage_value_unknown_table && type <= StorageValueTypes::storage_value_string_table);
            }
            
            inline static bool is_array_type(StorageValueTypes type) {
                return (type >= StorageValueTypes::storage_value_unknown_array && type <= StorageValueTypes::storage_value_string_array);
            }
        };
        
        
        struct StorageNullType {
            StorageNullType() : raw_storage(0) {}
            
            static const uint8_t    type;
            LUA_INTEGER raw_storage;
        };
        
        
        struct StorageIntType {
            StorageIntType() {}
            StorageIntType(LUA_INTEGER value) :raw_storage(value) {}
            
            static const uint8_t    type;
            LUA_INTEGER raw_storage;
        };
        
        
        struct StorageNumberType {
            StorageNumberType() {}
            StorageNumberType(double value) :raw_storage(value) {}
            
            static const uint8_t    type;
            double raw_storage;
        };
        
        
        struct StorageBoolType {
            StorageBoolType() {}
            StorageBoolType(bool value) :raw_storage(value) {}
            
            static const uint8_t    type;
            bool raw_storage;
        };
        
        
        struct StorageStringType {
            StorageStringType() {}
            StorageStringType(std::string value) :raw_storage(value) {}
            
            static const uint8_t    type;
            std::string raw_storage;
        };
        
        //table
        struct StorageIntTableType {
            StorageIntTableType() {}
            StorageIntTableType(const std::map<std::string, LUA_INTEGER>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, LUA_INTEGER> raw_storage_map;
        };
        
        
        struct StorageNumberTableType {
            StorageNumberTableType() {}
            StorageNumberTableType(const std::map<std::string, double>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, double> raw_storage_map;
        };
        
        
        struct StorageBoolTableType {
            StorageBoolTableType() {}
            StorageBoolTableType(const std::map<std::string, bool>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, bool> raw_storage_map;
        };
        
        struct StorageStringTableType {
            StorageStringTableType() {}
            StorageStringTableType(const std::map<std::string, std::string>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, std::string> raw_storage_map;
        };
        
        //array
        struct StorageIntArrayType {
            StorageIntArrayType() {}
            StorageIntArrayType(const std::map<std::string, LUA_INTEGER>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, LUA_INTEGER> raw_storage_map;
        };
        
        
        struct StorageNumberArrayType {
            StorageNumberArrayType() {}
            StorageNumberArrayType(const std::map<std::string, double>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, double> raw_storage_map;
        };
        
        
        struct StorageBoolArrayType {
            StorageBoolArrayType() {}
            StorageBoolArrayType(const std::map<std::string, bool>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, bool> raw_storage_map;
        };
        
        struct StorageStringArrayType {
            StorageStringArrayType() {}
            StorageStringArrayType(const std::map<std::string, std::string>& value_map) :raw_storage_map(value_map) {}
            
            static const uint8_t    type;
            std::map<std::string, std::string> raw_storage_map;
        };
        struct StorageDataChangeType {
            StorageDataType storage_before;
            StorageDataType storage_after;
        };
        typedef std::map<std::string, StorageDataChangeType> StorageDataChangeMap;
        typedef std::map<std::string, StorageDataChangeMap> AllStorageDataChange;
    }
} // lvm::blockchain


FC_REFLECT_ENUM(lvm::blockchain::StorageValueTypes,
                (storage_value_null)
                (storage_value_int)
                (storage_value_number)
                (storage_value_bool)
                (storage_value_string)
                (storage_value_unknown_table)
                (storage_value_int_table)
                (storage_value_number_table)
                (storage_value_bool_table)
                (storage_value_string_table)
                (storage_value_unknown_array)
                (storage_value_int_array)
                (storage_value_number_array)
                (storage_value_bool_array)
                (storage_value_string_array)
               )

FC_REFLECT(lvm::blockchain::StorageDataType,
           (storage_type)
           (storage_data)
          )

FC_REFLECT(lvm::blockchain::StorageDataChangeType,
           (storage_before)
           (storage_after)
          )

FC_REFLECT(lvm::blockchain::StorageNullType,
           (raw_storage)
          )

FC_REFLECT(lvm::blockchain::StorageIntType,
           (raw_storage)
          )

FC_REFLECT(lvm::blockchain::StorageBoolType,
           (raw_storage)
          )

FC_REFLECT(lvm::blockchain::StorageNumberType,
           (raw_storage)
          )

FC_REFLECT(lvm::blockchain::StorageStringType,
           (raw_storage)
          )

FC_REFLECT(lvm::blockchain::StorageIntTableType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageBoolTableType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageNumberTableType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageStringTableType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageIntArrayType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageBoolArrayType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageNumberArrayType,
           (raw_storage_map)
          )

FC_REFLECT(lvm::blockchain::StorageStringArrayType,
           (raw_storage_map)
          )
