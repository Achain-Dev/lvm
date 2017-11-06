#define ljsonrpclib_cpp

#include "glua/lauxlib.h"
#include "glua/lprefix.h"
#include "glua/lua.h"
#include "glua/lua_api.h"
#include "glua/lualib.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memory>
#include <mutex>

#undef LUA_JSONRPC_VERSION
#define LUA_JSONRPC_VERSION "1.0"

// TODO

static const luaL_Reg jsonrpclib[] = {
	// { "listen", lualib_http_listen },
	{ "version", nullptr },
	{ nullptr, nullptr }
};


LUAMOD_API int luaopen_jsonrpc(lua_State *L) {
	luaL_newlib(L, jsonrpclib);
	lua_pushstring(L, LUA_JSONRPC_VERSION);
	lua_setfield(L, -2, "version");
	return 1;
}
