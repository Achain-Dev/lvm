#include <stub/stub.hpp>
#include "base/easylogging++.h"
#include <glua/glua_complie_op.h>
#include <glua/GluaChainApi.hpp>
#include <fc/log/logger.hpp>

Stub::Stub() {
}

Stub::~Stub() {
}

#define CONTRACT_PATH  "D:/git/test_contract/"
#define CONTRACT_NAME  "USC_TEST1.glua"

void Stub::start() {
    // TODO: please start glua here for testing.
    thinkyoung::lua::api::global_glua_chain_api = new thinkyoung::lua::api::GluaChainApi();
    CompileOp op;
    op.compile_contract(CONTRACT_PATH CONTRACT_NAME);
}
