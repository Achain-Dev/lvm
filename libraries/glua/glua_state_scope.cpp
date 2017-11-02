#include <glua/lprefix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <utility>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>

#include <glua/thinkyoung_lua_api.h>
#include <glua/thinkyoung_lua_lib.h>

#include <glua/glua_lutil.h>
#include <glua/lobject.h>
#include <glua/lzio.h>
#include <glua/lundump.h>
#include <glua/lvm.h>
#include <glua/lapi.h>
#include <glua/lopcodes.h>
#include <glua/lparser.h>
#include <glua/lstate.h>
#include <glua/ldo.h>
#include <glua/ldebug.h>
#include <glua/lauxlib.h>
#include <glua/lualib.h>

using thinkyoung::lua::api::global_glua_chain_api;

namespace thinkyoung {
    namespace lua {
        namespace lib {
            GluaStateScope::GluaStateScope(bool use_contract)
                :_use_contract(use_contract) {
                this->_L = create_lua_state(use_contract);
            }
            GluaStateScope::GluaStateScope(const GluaStateScope &other) : _L(other._L) {}
            GluaStateScope::~GluaStateScope() {
                if (nullptr != _L)
                    close_lua_state(_L);
            }
            
            GluaStateValue GluaStateScope::get_value(const char *key) {
                return get_lua_state_value(_L, key);
            }
            
            void GluaStateScope::set_value(const char *key, GluaStateValue value, enum GluaStateValueType type) {
                set_lua_state_value(_L, key, value, type);
            }
            
            int GluaStateScope::get_instructions_executed_count() {
                return get_lua_state_instructions_executed_count(_L);
            }
            
            void GluaStateScope::add_global_c_function(const char *name, lua_CFunction func) {
                thinkyoung::lua::lib::add_global_c_function(_L, name, func);
            }
            void GluaStateScope::add_global_string_variable(const char *name, const char *str) {
                thinkyoung::lua::lib::add_global_string_variable(_L, name, str);
            }
            void GluaStateScope::add_global_int_variable(const char *name, lua_Integer num) {
                thinkyoung::lua::lib::add_global_int_variable(_L, name, num);
            }
            void GluaStateScope::add_global_number_variable(const char *name, lua_Number num) {
                thinkyoung::lua::lib::add_global_number_variable(_L, name, num);
            }
            void GluaStateScope::add_global_bool_variable(const char *name, bool value) {
                thinkyoung::lua::lib::add_global_bool_variable(_L, name, value);
            }
            void GluaStateScope::register_module(const char *name, lua_CFunction openmodule_func) {
                thinkyoung::lua::lib::register_module(_L, name, openmodule_func);
            }
            
            int GluaStateScope::execute_contract_api_by_address(const char *address, const char *api_name, const char *arg1, std::string *result_json_string) {
                return thinkyoung::lua::lib::execute_contract_api_by_address(_L, address, api_name, arg1, result_json_string);
            }
            
            bool GluaStateScope::execute_contract_init_by_address(const char *contract_address, const char *arg1, std::string *result_json_string) {
                return thinkyoung::lua::lib::execute_contract_init_by_address(_L, contract_address, arg1, result_json_string);
            }
            
            bool GluaStateScope::check_contract_bytecode_stream(GluaModuleByteStream *stream) {
                return thinkyoung::lua::lib::check_contract_bytecode_stream(_L, stream);
            }
            
            int *GluaStateScope::get_repl_state() {
                return thinkyoung::lua::lib::get_repl_state(_L);
            }
        }
    }
}