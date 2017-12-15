#include "glua/glua_chain_rpc_api.h"
#include "glua/lprefix.h"
#include "glua/lua_api.h"
#include "glua/lua_lib.h"
#include "glua/glua_lutil.h"
#include "glua/lstate.h"
#include "glua/lobject.h"
#include <glua/glua_chain_api.hpp>
#include <glua/glua_contractentry.hpp>
#include <task/task.hpp>
#include <glua/glua_task_mgr.h>

#include "storage/StorageTypes.hpp"

#include "fc/io/raw.hpp"

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
#include <set>

/**
* lua module injector header in lvm
*/
namespace lvm {
    namespace lua {
        namespace api {
            static int has_error = 0;
            
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
            
            std::shared_ptr<GluaModuleByteStream> GluaChainRpcApi::get_bytestream_from_code(lua_State *L, const Code& code) {
                if (code.byte_code.size() > LUA_MODULE_BYTE_STREAM_BUF_SIZE) {
                    return nullptr;
                }
                
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
            
            
            int GluaChainRpcApi::get_stored_contract_info_by_address(lua_State *L, const char *address, std::shared_ptr<GluaContractInfo> contract_info_ret) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_address(address);
                bool b_prefix_name = glua::util::starts_with(contract_address, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_address, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return
                    return 0;
                    
                } else {
                    std::string str_contract_address_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ADDRESS_IN_CHAIN).string_value);
                    std::string str_contract_id_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ID_IN_CHAIN).string_value);
                    
                    //TODO need debug to check whether contract_id is emptry
                    if (contract_address == str_contract_address_in_chain) {
                        Code* _code_ptr = (Code *)(lvm::lua::lib::get_lua_state_value(L, PTR_ENTRY_CODE_IN_CHAIN).pointer_value);
                        
                        if (_code_ptr) {
                            Code& code = *_code_ptr;
                            contract_info_ret->contract_apis.clear();
                            std::copy(code.abi.begin(), code.abi.end(), std::back_inserter(contract_info_ret->contract_apis));
                            std::copy(code.offline_abi.begin(), code.offline_abi.end(), std::back_inserter(contract_info_ret->contract_apis));
                            return 1;
                        }
                        
                        // _code_ptr not null just go ahead to get the code from chain
                    }
                    
                    {
                        void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                        
                        if (!tmp_statevalue_ptr) {
                            return 0;
                        }
                        
                        LuaRequestTask p;
                        p.method = GET_STORED_CONTRACT_INFO_BY_ADDRESS;
                        p.params.push_back(fc::raw::pack<std::string>(contract_address));
                        p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                        LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                        
                        if (result.err_num != 0 || result.params.size() < 1) {
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return 0;
                        }
                        
                        std::set<std::string> abi;
                        std::set<std::string> offline_abi;
                        
                        if (fc::raw::unpack<bool>(result.params[0]) == 0) {
                            return 0;
                        }
                        
                        FC_ASSERT(result.params.size() > 2);
                        abi = fc::raw::unpack<std::set<std::string>>(result.params[1]);
                        offline_abi = fc::raw::unpack<std::set<std::string>>(result.params[2]);
                        contract_info_ret->contract_apis.clear();
                        std::copy(abi.begin(), abi.end(), std::back_inserter(contract_info_ret->contract_apis));
                        std::copy(offline_abi.begin(), offline_abi.end(), std::back_inserter(contract_info_ret->contract_apis));
                        return 1;
                    }
                }
            }
            
            void GluaChainRpcApi::get_contract_address_by_name(lua_State *L, const char *name, char *address, size_t *address_size) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_name(name);
                bool b_prefix_name = glua::util::starts_with(contract_name, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_name, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return
                    return;
                    
                } else {
                    std::string str_contract_address_in_chain(lvm::lua::lib::get_lua_state_value (L, STR_CONTRACT_ADDRESS_IN_CHAIN).string_value);
                    std::string str_contract_id_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ID_IN_CHAIN).string_value);
                    
                    //the same contract name just return contract id
                    //TODO need debug to check whether contract_id is emptry
                    if (contract_name == str_contract_address_in_chain) {
                        *address_size = str_contract_id_in_chain.length();
                        strncpy(address, str_contract_id_in_chain.c_str(), CONTRACT_ID_MAX_LENGTH - 1);
                        address[CONTRACT_ID_MAX_LENGTH - 1] = '\0';
                        return;
                        
                    } else {
                        void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                        
                        if (!tmp_statevalue_ptr) {
                            return ;
                        }
                        
                        LuaRequestTask p;
                        p.method = GET_CONTRACT_ADDRESS_BY_NAME;
                        p.params.push_back(fc::raw::pack<std::string>(contract_name));
                        p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                        LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                        
                        if (result.err_num != 0 || result.params.size() < 1) {
                            return;
                        }
                        
                        std::string address_str = fc::raw::unpack<std::string>(result.params[0]);
                        *address_size = address_str.length();
                        strncpy(address, address_str.c_str(), CONTRACT_ID_MAX_LENGTH - 1);
                        address[CONTRACT_ID_MAX_LENGTH - 1] = '\0';
                        return;
                    }
                }
            }
            
            bool GluaChainRpcApi::check_contract_exist_by_address(lua_State *L, const char *address) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_address(address);
                bool b_prefix_name = glua::util::starts_with(contract_address, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_address, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return false
                    return false;
                    
                } else {
                    std::string str_contract_address_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ADDRESS_IN_CHAIN).string_value);
                    std::string str_contract_id_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ID_IN_CHAIN).string_value);
                    
                    //the same contract name just return contract id
                    //TODO need debug to check whether contract_id in condition
                    if (contract_address == str_contract_address_in_chain) {
                        if (str_contract_id_in_chain.empty ()) {
                            return false;
                        }
                        
                        return true;
                        
                    } else {
                        void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                        
                        if (!tmp_statevalue_ptr) {
                            return false;
                        }
                        
                        LuaRequestTask p;
                        p.method = CHECK_CONTRACT_EXIST_BY_ADDRESS;
                        p.params.push_back(fc::raw::pack<std::string>(contract_address));
                        p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                        LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                        
                        if (result.err_num != 0 || result.params.size() < 1) {
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return false;
                        }
                        
                        return fc::raw::unpack<bool>(result.params[0]);
                    }
                }
            }
            
            bool GluaChainRpcApi::check_contract_exist(lua_State *L, const char *name) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_name(name);
                bool b_prefix_name = glua::util::starts_with(contract_name, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_name, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return false
                    return false;
                    
                } else {
                    std::string str_contract_address_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ADDRESS_IN_CHAIN).string_value);
                    std::string str_contract_id_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ID_IN_CHAIN).string_value);
                    
                    //TODO no need to check contract id
                    if (contract_name == str_contract_address_in_chain) {
                        // this api will not check the contract id
                        return true;
                        
                    } else {
                        void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                        
                        if (!tmp_statevalue_ptr) {
                            return false;
                        }
                        
                        LuaRequestTask p;
                        p.method = CHECK_CONTRACT_EXIST;
                        p.params.push_back(fc::raw::pack<std::string>(contract_name));
                        p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                        LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                        
                        if (result.err_num != 0 || result.params.size() < 1) {
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return false;
                        }
                        
                        return fc::raw::unpack<bool>(p.params[0]);
                    }
                }
            }
            
            /**
            * load contract lua byte stream from thinkyoung api
            */
            std::shared_ptr<GluaModuleByteStream> GluaChainRpcApi::open_contract(lua_State *L, const char *name) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_name(name);
                bool b_prefix_name = glua::util::starts_with(contract_name, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_name, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return
                    return nullptr;
                    
                } else {
                    std::string str_contract_address_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ADDRESS_IN_CHAIN).string_value);
                    std::string str_contract_id_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ID_IN_CHAIN).string_value);
                    
                    //TODO need debug to check whether contract_id is emptry
                    if (contract_name == str_contract_address_in_chain) {
                        Code* _code_ptr = (Code *)(lvm::lua::lib::get_lua_state_value(L, PTR_ENTRY_CODE_IN_CHAIN).pointer_value);
                        
                        if (_code_ptr) {
                            return get_bytestream_from_code(L, *_code_ptr);
                        }
                        
                        // _code_ptr not null just go ahead to get the code from chain
                    }
                    
                    {
                        void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                        
                        if (!tmp_statevalue_ptr) {
                            return nullptr;
                        }
                        
                        LuaRequestTask p;
                        p.method = OPEN_CONTRACT;
                        p.params.push_back(fc::raw::pack<std::string>(contract_name));
                        p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                        LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                        
                        if (result.err_num != 0 || result.params.size() < 1) {
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return nullptr;
                        }
                        
                        if (fc::raw::unpack<bool>(result.params[0])) {
                            Code code = fc::raw::unpack<Code>(result.params[1]);
                            return get_bytestream_from_code(L, code);
                        }
                        
                        return nullptr;
                    }
                }
            }
            
            
            std::shared_ptr<GluaModuleByteStream> GluaChainRpcApi::open_contract_by_address(lua_State *L, const char *address) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_address(address);
                bool b_prefix_name = glua::util::starts_with(contract_address, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_address, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return
                    return nullptr;
                    
                } else {
                    std::string str_contract_address_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ADDRESS_IN_CHAIN).string_value);
                    std::string str_contract_id_in_chain(lvm::lua::lib::get_lua_state_value(L, STR_CONTRACT_ID_IN_CHAIN).string_value);
                    
                    //the same contract name just return contract id
                    //TODO need debug to check whether contract_id is emptry
                    if (contract_address == str_contract_address_in_chain) {
                        Code* _code_ptr = (Code *)(lvm::lua::lib::get_lua_state_value(L, PTR_ENTRY_CODE_IN_CHAIN).pointer_value);
                        
                        if (_code_ptr) {
                            return get_bytestream_from_code(L, *_code_ptr);
                        }
                        
                        // _code_ptr not null just go ahead to get the code from chain
                    }
                    
                    {
                        void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                        
                        if (!tmp_statevalue_ptr) {
                            return nullptr;
                        }
                        
                        LuaRequestTask p;
                        p.method = OPEN_CONTRACT_BY_ADDRESS;
                        p.params.push_back(fc::raw::pack<std::string>(contract_address));
                        p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                        LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                        
                        if (result.err_num != 0 || result.params.size() < 1) {
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return nullptr;
                        }
                        
                        if (fc::raw::unpack<bool>(result.params[0])) {
                            Code code = fc::raw::unpack<Code>(result.params[1]);
                            return get_bytestream_from_code(L, code);
                        }
                        
                        return nullptr;
                    }
                }
            }
            
            
            GluaStorageValue GluaChainRpcApi::get_storage_value_from_thinkyoung_by_address(lua_State *L, const char *contract_address, std::string name) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_addr(contract_address);
                GluaStorageValue null_storage;
                bool b_prefix_name = glua::util::starts_with(contract_addr, std::string(ADDRESS_CONTRACT_PREFIX)) ||
                                     glua::util::starts_with(contract_addr, std::string(STREAM_CONTRACT_PREFIX));
                                     
                if (b_prefix_name) {
                    // no store in database with prefix @pointer_ and @stream_  ; just return
                    return null_storage;
                }
                
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return null_storage;
                }
                
                null_storage.type = lvm::blockchain::StorageValueTypes::storage_value_null;
                std::string storage_name(name);
                LuaRequestTask p;
                p.method = GET_STORAGE_VALUE_FROM_THINKYOUNG;
                p.params.push_back(fc::raw::pack<std::string>(contract_address));
                p.params.push_back(fc::raw::pack<std::string>(storage_name));
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return null_storage;
                }
                
                lvm::blockchain::StorageDataType storage_data = fc::raw::unpack<lvm::blockchain::StorageDataType>(result.params[0]);
                return lvm::blockchain::StorageDataType::create_lua_storage_from_storage_data(L, storage_data);
            }
            
            
            bool GluaChainRpcApi::commit_storage_changes_to_thinkyoung(lua_State *L, AllContractsChangesMap &changes) {
                if (changes.empty()) {
                    return false;
                }
                
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return false;
                }
                
                using namespace blockchain;
                lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                LuaRequestTask p;
                p.method = COMMIT_STORAGE_CHANGES_TO_THINKYOUNG;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                AllStorageDataChange all_change;
                
                for (auto all_con_chg_iter = changes.begin(); all_con_chg_iter != changes.end(); ++all_con_chg_iter) {
                    std::string contract_id = all_con_chg_iter->first;
                    ContractChangesMap contract_change = *(all_con_chg_iter->second);
                    StorageDataChangeMap storage_change_map;
                    
                    for (auto con_chg_iter = contract_change.begin(); con_chg_iter != contract_change.end(); ++con_chg_iter) {
                        StorageDataChangeType storage_change;
                        std::string contract_name = con_chg_iter->first;
                        storage_change.storage_before = StorageDataType::get_storage_data_from_lua_storage(con_chg_iter->second.before);
                        storage_change.storage_after = StorageDataType::get_storage_data_from_lua_storage(con_chg_iter->second.after);
                        storage_change_map.insert(make_pair(contract_name, storage_change));
                    }
                    
                    all_change.insert(make_pair(contract_id, storage_change_map));
                }
                
                p.params.push_back(fc::raw::pack(all_change));
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 ) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return false;
                }
                
                //return fc::raw::unpack<bool>(result.params[0]);
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
                        
                        if (object_addr == 0) {
                            continue;
                        }
                        
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
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return false;
                }
                
                std::string contract_addr(contract_address);
                std::string to_addr(to_address);
                LuaRequestTask p;
                p.method = TRANSFER_FROM_CONTRACT_TO_ADDRESS;
                p.params.push_back(fc::raw::pack<std::string>(contract_addr));
                p.params.push_back(fc::raw::pack<std::string>(to_addr));
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    switch (result.err_num) {
                        case 31302:
                            return -2;
                            
                        case 31003: //unknown balance entry
                            return -5;
                            
                        case 31004:
                            return -5;
                            
                        default:
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return -1;
                    }
                }
                
                return fc::raw::unpack<int>(result.params[0]);
            }
            lua_Integer GluaChainRpcApi::transfer_from_contract_to_public_account(lua_State *L, const char *contract_address, const char *to_account_name,
                    const char *asset_type, int64_t amount) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return false;
                }
                
                std::string contract_addr(contract_address);
                std::string to_acc_name(to_account_name);
                LuaRequestTask p;
                p.method = TRANSFER_FROM_CONTRACT_TO_PUBLIC_ACCOUNT;
                p.params.push_back(fc::raw::pack<std::string>(contract_addr));
                p.params.push_back(fc::raw::pack<std::string>(to_acc_name));
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    switch (result.err_num) {
                        case 31302:
                            return -2;
                            
                        case 31003: //unknown balance entry
                            return -5;
                            
                        case 31004:
                            return -5;
                            
                        default:
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return -1;
                    }
                }
                
                return fc::raw::unpack<int>(result.params[0]);
            }
            int64_t GluaChainRpcApi::get_contract_balance_amount(lua_State *L, const char *contract_address, const char* asset_symbol) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                std::string contract_addr(contract_address);
                std::string asset_sym(asset_symbol);
                LuaRequestTask p;
                p.method = GET_CONTRACT_BALANCE_AMOUNT;
                p.params.push_back(fc::raw::pack<std::string>(contract_addr));
                p.params.push_back(fc::raw::pack<std::string>(asset_sym));
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    switch (result.err_num) {
                        case 30028://invalid_address
                            return -2;
                            
                        //case 31003://unknown_balance_entry
                        //    return -3;
                        case 31303:
                            return -1;
                            
                        default:
                            L->force_stopping = true;
                            L->exit_code = LUA_API_INTERNAL_ERROR;
                            return -4;
                            break;
                    }
                }
                
                return fc::raw::unpack<int>(result.params[0]);
            }
            int64_t GluaChainRpcApi::get_transaction_fee(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                LuaRequestTask p;
                p.method = GET_TRANSACTION_FEE;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return -2;
                }
                
                if (auto i = fc::raw::unpack<int64_t>(result.params[0]) < 0) {
                    return i;
                }
                
                return fc::raw::unpack<int64_t>(result.params[1]);
            }
            uint32_t GluaChainRpcApi::get_chain_now(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                LuaRequestTask p;
                p.method = GET_CHAIN_NOW;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return 0;
                }
                
                return fc::raw::unpack<uint32_t>(result.params[0]);
            }
            uint32_t GluaChainRpcApi::get_chain_random(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return false;
                }
                
                LuaRequestTask p;
                p.method = GET_CHAIN_RANDOM;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return 0;
                }
                
                return fc::raw::unpack<uint32_t>(result.params[0]);
            }
            std::string GluaChainRpcApi::get_transaction_id(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return "";
                }
                
                LuaRequestTask p;
                p.method = GET_TRANSACTION_ID;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return "";
                }
                
                return fc::raw::unpack<std::string>(result.params[0]);
            }
            uint32_t GluaChainRpcApi::get_header_block_num(lua_State *L) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return 0;
                }
                
                LuaRequestTask p;
                p.method = GET_HEADER_BLOCK_NUM;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return 0;
                }
                
                return fc::raw::unpack<uint32_t>(result.params[0]);
            }
            uint32_t GluaChainRpcApi::wait_for_future_random(lua_State *L, int next) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                LuaRequestTask p;
                p.method = WAIT_FOR_FUTURE_RANDOM;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                p.params.push_back(fc::raw::pack(next));
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return 0;
                }
                
                return fc::raw::unpack<uint32_t>(result.params[0]);
            }
            //获取指定块与之前50块的pre_secret hash出的结果，该值在指定块被产出的上一轮出块时就已经确定，而无人可知，无法操控
            //如果希望使用该值作为随机值，以随机值作为其他数据的选取依据时，需要在目标块被产出前确定要被筛选的数据
            //如投注彩票，只允许在目标块被产出前投注
            int32_t GluaChainRpcApi::get_waited(lua_State *L, uint32_t num) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return false;
                }
                
                LuaRequestTask p;
                p.method = GET_WAITED;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                p.params.push_back(fc::raw::pack(num));
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0 || result.params.size() < 1) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return -1;
                }
                
                if (auto i = fc::raw::unpack<int32_t>(result.params[0]) < 0) {
                    return i;
                }
                
                return fc::raw::unpack<int32_t>(result.params[1]);
                //string get_wait
            }
            void GluaChainRpcApi::emit(lua_State *L, const char* contract_id, const char* event_name, const char* event_param) {
                lvm::lua::lib::increment_lvm_instructions_executed_count(L, CHAIN_GLUA_API_EACH_INSTRUCTIONS_COUNT - 1);
                void * tmp_statevalue_ptr = lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value;
                
                if (!tmp_statevalue_ptr) {
                    return ;
                }
                
                std::string cact_id(contract_id);
                std::string ev_name(event_name);
                std::string ev_param(event_param);
                LuaRequestTask p;
                p.method = EMIT;
                p.statevalue = reinterpret_cast<intptr_t>(lvm::lua::lib::get_lua_state_value(L, "evaluate_state").pointer_value);
                p.params.push_back(fc::raw::pack(cact_id));
                p.params.push_back(fc::raw::pack(ev_name));
                p.params.push_back(fc::raw::pack(ev_param));
                LuaRequestTaskResult result = GluaTaskMgr::get_glua_task_mgr()->lua_request(p);
                
                if (result.err_num != 0) {
                    L->force_stopping = true;
                    L->exit_code = LUA_API_INTERNAL_ERROR;
                    return ;
                }
            }
        }
    }
}
