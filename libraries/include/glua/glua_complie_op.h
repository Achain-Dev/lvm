#ifndef GLUA_COMPILE_OP
#define GLUA_COMPILE_OP

#include <glua/lua_api.h>
#include <glua/lua_lib.h>

#include <fc/filesystem.hpp>

class CompileOp {
  public:
    CompileOp();
    ~CompileOp();
    fc::path compile_lua(const fc::path&, bool compile_contract) const;
    
  private:
    int save_code_to_file(const fc::string& name, GluaModuleByteStream *stream, char* err_msg) const;
};
#endif
