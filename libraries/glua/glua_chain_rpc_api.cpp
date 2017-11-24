#include "glua/glua_chain_rpc_api.h"

#include "glua/lprefix.h"
#include "glua/lua_api.h"
#include "glua/lua_lib.h"
#include "glua/glua_lutil.h"
#include "glua/lstate.h"
#include "glua/lobject.h"
#include <glua/glua_chain_api.hpp>
#include <glua/glua_contractentry.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <utility>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include <mutex>

/**
* lua module injector header in lvm
*/
namespace lvm {
    namespace lua {
        namespace api {
            // TODO: all these apis need TODO
            
            static int has_error = 0;
            
            static std::string get_file_name_str_from_contract_module_name(std::string name) {
                std::stringstream ss;
                ss << "lvm_contract_" << name;
                return ss.str();
            }
            
            /**
            * whether exception happen in L
            */
            bool GluaChainRpcApi::has_exception(lua_State *L) {
                return has_error ? true : false;
            }
            
            /**
            * clear exception marked
            */
            void GluaChainRpcApi::clear_exceptions(lua_State *L) {
                has_error = 0;
            }
            
            /**
            * when exception happened, use this api to tell lvm
            * @param L the lua stack
            * @param code error code, 0 is OK, other is different error
            * @param error_format error info string, will be released by lua
            * @param ... error arguments
            */
            void GluaChainRpcApi::throw_exception(lua_State *L, int code, const char *error_format, ...) {
                has_error = 1;
                char *msg = (char*)lua_malloc(L, LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH);
                memset(msg, 0x0, LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH);
                va_list vap;
                va_start(vap, error_format);
                vsnprintf(msg, LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH, error_format, vap);
                va_end(vap);
                
                if (strlen(msg) > LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH - 1) {
                    msg[LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH - 1] = 0;
                }
                
                lua_set_compile_error(L, msg);
                //如果上次的exception code为THINKYOUNG_API_LVM_LIMIT_OVER_ERROR, 不能被其他异常覆盖
                //只有调用clear清理后，才能继续记录异常
                int last_code = lua::lib::get_lua_state_value(L, "exception_code").int_value;
                
                if (last_code == LVM_API_LVM_LIMIT_OVER_ERROR
                        && code != LVM_API_LVM_LIMIT_OVER_ERROR) {
                    return;
                }
                
                GluaStateValue val_code;
                val_code.int_value = code;
                GluaStateValue val_msg;
                val_msg.string_value = msg;
                lua::lib::set_lua_state_value(L, "exception_code", val_code, GluaStateValueType::LUA_STATE_VALUE_INT);
                lua::lib::set_lua_state_value(L, "exception_msg", val_msg, GluaStateValueType::LUA_STATE_VALUE_STRING);
            }
            
            /**
            * check whether the contract apis limit over, in this lua_State
            * @param L the lua stack
            * @return TRUE(1 or not 0) if over limit(will break the vm), FALSE(0) if not over limit
            */
            int GluaChainRpcApi::check_contract_api_instructions_over_limit(lua_State *L) {
                return 0; // FIXME: need fill by thinkyoung api
            }
            
            int GluaChainRpcApi::get_stored_contract_info_by_address(lua_State *L, const char *address, std::shared_ptr<GluaContractInfo> contract_info_ret) {
                return 1;
            }
            
            void GluaChainRpcApi::get_contract_address_by_name(lua_State *L, const char *name, char *address, size_t *address_size) {
            }
            
            bool GluaChainRpcApi::check_contract_exist_by_address(lua_State *L, const char *address) {
                return true;
            }
            
            bool GluaChainRpcApi::check_contract_exist(lua_State *L, const char *name) {
                return true;
            }
            
            std::shared_ptr<GluaModuleByteStream> GluaChainRpcApi::get_bytestream_from_code(lua_State *L, const Code& code) {
                if (code.byte_code.size() > LUA_MODULE_BYTE_STREAM_BUF_SIZE)
                    return NULL;
                    
                auto p_luamodule = std::make_shared<GluaModuleByteStream>();
                p_luamodule->is_bytes = true;
                p_luamodule->buff.resize(code.byte_code.size());
                memcpy(p_luamodule->buff.data(), code.byte_code.data(), code.byte_code.size());
                p_luamodule->contract_name = "";
                p_luamodule->contract_apis.clear();
                std::copy(code.abi.begin(), code.abi.end(), std::back_inserter(p_luamodule->contract_apis));
                p_luamodule->contract_emit_events.clear();
                std::copy(code.offline_abi.begin(), code.offline_abi.end(), std::back_inserter(p_luamodule->offline_apis));
                p_luamodule->contract_emit_events.clear();
                std::copy(code.events.begin(), code.events.end(), std::back_inserter(p_luamodule->contract_emit_events));
                p_luamodule->contract_storage_properties.clear();
                std::copy(code.storage_properties.begin(), code.storage_properties.end(), std::inserter(p_luamodule->contract_storage_properties, p_luamodule->contract_storage_properties.begin()));
                return p_luamodule;
            }
            /**
            * load contract lua byte stream from thinkyoung api
            */
            std::shared_ptr<GluaModuleByteStream> GluaChainRpcApi::open_contract(lua_State *L, const char *name) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return NULL;
            }
            
            std::shared_ptr<GluaModuleByteStream> GluaChainRpcApi::open_contract_by_address(lua_State *L, const char *address) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return NULL;
            }
            
            GluaStorageValue GluaChainRpcApi::get_storage_value_from_thinkyoung_by_address(lua_State *L, const char *contract_address, std::string name) {
                GluaStorageValue null_storage;
                null_storage.type = lvm::blockchain::StorageValueTypes::storage_value_null;
                return null_storage;
            }
            
            bool GluaChainRpcApi::commit_storage_changes_to_thinkyoung(lua_State *L, AllContractsChangesMap &changes) {
                return true;
            }
            
            //not use
            bool GluaChainRpcApi::register_storage(lua_State *L, const char *contract_name, const char *name) {
                // TODO
                printf("registered storage %s[%s] to thinkyoung\n", contract_name, name);
                return true;
            }
            
            intptr_t GluaChainRpcApi::register_object_in_pool(lua_State *L, intptr_t object_addr, GluaOutsideObjectTypes type) {
                auto node = lvm::lua::lib::get_lua_state_value_node(L, GLUA_OUTSIDE_OBJECT_POOLS_KEY);
                std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>> *object_pools = nullptr;
                
                if (node.type == GluaStateValueType::LUA_STATE_VALUE_nullptr) {
                    node.type = GluaStateValueType::LUA_STATE_VALUE_POINTER;
                    object_pools = new std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>>();
                    node.value.pointer_value = (void*)object_pools;
                    lvm::lua::lib::set_lua_state_value(L, GLUA_OUTSIDE_OBJECT_POOLS_KEY, node.value, node.type);
                    
                } else {
                    object_pools = (std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>> *) node.value.pointer_value;
                }
                
                if (object_pools->find(type) == object_pools->end()) {
                    object_pools->emplace(std::make_pair(type, std::make_shared<std::map<intptr_t, intptr_t>>()));
                }
                
                auto pool = (*object_pools)[type];
                auto object_key = object_addr;
                (*pool)[object_key] = object_addr;
                return object_key;
            }
            
            intptr_t GluaChainRpcApi::is_object_in_pool(lua_State *L, intptr_t object_key, GluaOutsideObjectTypes type) {
                auto node = lvm::lua::lib::get_lua_state_value_node(L, GLUA_OUTSIDE_OBJECT_POOLS_KEY);
                // Map<type, Map<object_key, object_addr>>
                std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>> *object_pools = nullptr;
                
                if (node.type == GluaStateValueType::LUA_STATE_VALUE_nullptr) {
                    return 0;
                    
                } else {
                    object_pools = (std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>> *) node.value.pointer_value;
                }
                
                if (object_pools->find(type) == object_pools->end()) {
                    object_pools->emplace(std::make_pair(type, std::make_shared<std::map<intptr_t, intptr_t>>()));
                }
                
                auto pool = (*object_pools)[type];
                return (*pool)[object_key];
            }
            
            void GluaChainRpcApi::release_objects_in_pool(lua_State *L) {
                auto node = lvm::lua::lib::get_lua_state_value_node(L, GLUA_OUTSIDE_OBJECT_POOLS_KEY);
                // Map<type, Map<object_key, object_addr>>
                std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>> *object_pools = nullptr;
                
                if (node.type == GluaStateValueType::LUA_STATE_VALUE_nullptr) {
                    return;
                }
                
                object_pools = (std::map<GluaOutsideObjectTypes, std::shared_ptr<std::map<intptr_t, intptr_t>>> *) node.value.pointer_value;
                
                // TODO: 对于object_pools中不同类型的对象，分别释放
                for (const auto &p : *object_pools) {
                    auto type = p.first;
                    auto pool = p.second;
                    
                    for (const auto &object_item : *pool) {
                        auto object_key = object_item.first;
                        auto object_addr = object_item.second;
                        
                        if (object_addr == 0)
                            continue;
                            
                        switch (type) {
                            case GluaOutsideObjectTypes::OUTSIDE_STREAM_STORAGE_TYPE: {
                                auto stream = (lvm::lua::lib::GluaByteStream*) object_addr;
                                delete stream;
                            }
                            break;
                            
                            default: {
                                continue;
                            }
                        }
                    }
                }
                
                delete object_pools;
                GluaStateValue null_state_value;
                null_state_value.int_value = 0;
                lvm::lua::lib::set_lua_state_value(L, GLUA_OUTSIDE_OBJECT_POOLS_KEY, null_state_value, GluaStateValueType::LUA_STATE_VALUE_nullptr);
            }
            
            lua_Integer GluaChainRpcApi::transfer_from_contract_to_address(lua_State *L, const char *contract_address, const char *to_address,
                    const char *asset_type, int64_t amount) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return 0;
            }
            
            lua_Integer GluaChainRpcApi::transfer_from_contract_to_public_account(lua_State *L, const char *contract_address, const char *to_account_name,
                    const char *asset_type, int64_t amount) {
                return -1;
            }
            
            int64_t GluaChainRpcApi::get_contract_balance_amount(lua_State *L, const char *contract_address, const char* asset_symbol) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return -1;
            }
            
            int64_t GluaChainRpcApi::get_transaction_fee(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return -2;
            }
            
            uint32_t GluaChainRpcApi::get_chain_now(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return -2;
            }
            uint32_t GluaChainRpcApi::get_chain_random(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return 0;
            }
            
            std::string GluaChainRpcApi::get_transaction_id(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return "";
            }
            
            uint32_t GluaChainRpcApi::get_header_block_num(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return 0;
            }
            
            uint32_t GluaChainRpcApi::wait_for_future_random(lua_State *L, int next) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return 0;
            }
            //获取指定块与之前50块的pre_secret hash出的结果，该值在指定块被产出的上一轮出块时就已经确定，而无人可知，无法操控
            //如果希望使用该值作为随机值，以随机值作为其他数据的选取依据时，需要在目标块被产出前确定要被筛选的数据
            //如投注彩票，只允许在目标块被产出前投注
            int32_t GluaChainRpcApi::get_waited(lua_State *L, uint32_t num) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                return -1;
                //string get_wait
            }
            
            void GluaChainRpcApi::emit(lua_State *L, const char* contract_id, const char* event_name, const char* event_param) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
            }
        }
    }
}
