#ifndef _GLUA_LOADER_H_
#define _GLUA_LOADER_H_

#include "glua/lprefix.h"
#include "glua/llimits.h"
#include "glua/lstate.h"
#include "glua/lua.h"
#include "glua/lua_api.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <list>

namespace thinkyoung
{
    namespace lua
    {
        namespace parser
        {

            class LuaLoader
            {
            private:
                GluaModuleByteStream *_stream; // byte code stream
            public:
                LuaLoader(GluaModuleByteStream *stream);
                ~LuaLoader();

                void load_bytecode(); // load bytecode stream to AST
            };

        }
    }
}

#endif