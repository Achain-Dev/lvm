#include <stub/stub.hpp>
#include "base/easylogging++.h"
#include <rpc/rpc_mgr.hpp>

Stub::Stub() {
}

Stub::~Stub() {
}

void Stub::start() {
    // TODO: please start glua here for testing.
#if 0
    RpcMgr* my = new RpcMgr();
    my->set_endpoint(std::string("127.0.0.1"), 65000);
    
    try {
        my->start();
        
    } catch (fc::exception& e) {
    }
    
#endif
}
