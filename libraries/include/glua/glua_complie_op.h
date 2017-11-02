#ifndef GLUA_COMPILE_OP
#define GLUA_COMPILE_OP

#pragma once

#include <fc\filesystem.hpp>
#include"glua/thinkyoung_lua_api.h"
#include"glua/thinkyoung_lua_lib.h"

class CompileOp {
  public:
    CompileOp();
    ~CompileOp();
    fc::path compile_contract(const fc::path&) const;
    
  private:
    int save_code_to_file(const fc::string& name, GluaModuleByteStream *stream, char* err_msg) const;
};

#endif
