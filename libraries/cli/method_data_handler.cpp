#include <cli/method_data_handler.hpp>

MethodDataHandler::MethodDataHandler() {
    register_methods();
}

MethodDataHandler::~MethodDataHandler() {
    unregister_methods();
}

MethodData* MethodDataHandler::find_method_data(const std::string& method) {
    MethodData* method_data = nullptr;
    std::map<std::string, MethodData*>::iterator iter =
        _map_method_datas.find(method);
    if (iter != _map_method_datas.end()) {
        method_data = iter->second;
    }
    
    return method_data;
}

void MethodDataHandler::register_methods() {
    MethodData* compile_method_data = new MethodData;
    compile_method_data->name = "compile_contract";
    compile_method_data->description = "compile the contract";
    _map_method_datas["compile_contract"] = compile_method_data;

    MethodData* register_method_data = new MethodData;
    register_method_data->name = "register_contract";
    register_method_data->description = "register the contract";
    _map_method_datas["register_contract"] = register_method_data;

    MethodData* register_testing_method_data = new MethodData;
    register_testing_method_data->name = "register_contract_testing";
    register_testing_method_data->description = "register testing the contract";
    _map_method_datas["register_contract_testing"] = register_testing_method_data;

    MethodData* call_method_data = new MethodData;
    call_method_data->name = "call_contract";
    call_method_data->description = "call the contract";
    _map_method_datas["call_contract"] = call_method_data;

    MethodData* call_testing_method_data = new MethodData;
    call_testing_method_data->name = "call_contract_testing";
    call_testing_method_data->description = "call testing the contract";
    _map_method_datas["call_contract_testing"] = call_testing_method_data;

    MethodData* upgrade_method_data = new MethodData;
    upgrade_method_data->name = "upgrade_contract";
    upgrade_method_data->description = "upgrade the contract";
    _map_method_datas["upgrade_contract"] = upgrade_method_data;

    MethodData* destroy_method_data = new MethodData;
    destroy_method_data->name = "destroy_contract";
    destroy_method_data->description = "destroy the contract";
    _map_method_datas["destroy_contract"] = destroy_method_data;
}

void MethodDataHandler::unregister_methods() {
    std::map<std::string, MethodData*>::iterator iter;
    for (iter = _map_method_datas.begin(); iter != _map_method_datas.end();
        iter++) {
        MethodData* method_data = iter->second;
        if (method_data) {
            delete method_data;
        }
    }
    _map_method_datas.clear();
}
