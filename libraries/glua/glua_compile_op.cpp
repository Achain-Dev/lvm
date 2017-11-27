#include "base/common_api.hpp"
#include "base/exceptions.hpp"
#include <glua/glua_complie_op.h>
#include <glua/glua_lutil.h>
#include <glua/lua_lib.h>

#include<fc/exception/exception.hpp>
#include<fc/string.hpp>
#include<boost/uuid/sha1.hpp>

#include<string>

CompileOp::CompileOp() {
}


CompileOp::~CompileOp() {
}

fc::path CompileOp::compile_lua(const fc::path& filename, bool compile_contract) const {
    using namespace fc;
    
    // if file not exist
    if (!fc::exists(filename))
        FC_THROW_EXCEPTION(fc::file_not_found_exception, "the file not exist!");
        
    char err_msg[LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH + 1] = "\0";
    string filename_str = filename.string();
    string out_filename;
    size_t pos;
    pos = filename_str.find_last_of('.');
    std::string exe_file_name = compile_contract ? ".gpc" : ".script";
    
    if ((pos != string::npos) && (filename_str.substr(pos) == ".glua" || filename_str.substr(pos) == ".lua")) {
        out_filename = filename_str.substr(0, pos) + exe_file_name;
        
    } else {
        FC_THROW_EXCEPTION(lvm::global_exception::invalid_contract_filename, "contract source file name should end with .lua or .glua");
    }
    
    auto lua_module_ptr = std::make_shared<GluaModuleByteStream>();
    FC_ASSERT(lua_module_ptr, "Alloc memory for GluaModuleByteStream failed!");
    
    if (compile_contract) {
        glua::util::TimeDiff time_diff;
        time_diff.start();
        
        if (!lvm::lua::lib::compile_contract_to_stream(filename_str.c_str(), lua_module_ptr.get(), err_msg, nullptr, USE_TYPE_CHECK)) {
            err_msg[LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH] = '\0';
            FC_THROW_EXCEPTION(lvm::global_exception::compile_contract_fail, err_msg);
        }
        
        time_diff.end();
        std::cout << "compile using time " << time_diff.diff_timestamp() << "s" << std::endl;
        
    } else {
        lvm::lua::lib::GluaStateScope sco(false);
        
        if (!lvm::lua::lib::compilefile_to_stream(sco.L(), filename.generic_string().c_str(), lua_module_ptr.get(), err_msg, USE_TYPE_CHECK)) {
            FC_THROW_EXCEPTION(lvm::global_exception::compile_script_fail, err_msg);
        }
    }
    
    if (save_code_to_file(out_filename, lua_module_ptr.get(), err_msg) < 0) {
        err_msg[LUA_EXCEPTION_MULTILINE_STRNG_MAX_LENGTH] = '\0';
        FC_THROW_EXCEPTION(lvm::global_exception::save_bytecode_to_gpcfile_fail, err_msg);
    }
    
    return fc::path(out_filename);
}

int CompileOp::save_code_to_file(const fc::string& name, GluaModuleByteStream *stream, char* err_msg) const {
    boost::uuids::detail::sha1 sha;
    unsigned int digest[5];
    GluaModuleByteStream* p_new_stream = new GluaModuleByteStream();
    
    if (NULL == p_new_stream) {
        strcpy(err_msg, "malloc GluaModuleByteStream fail");
        return -1;
    }
    
    p_new_stream->is_bytes = stream->is_bytes;
    p_new_stream->buff = stream->buff;
    
    for (int i = 0; i < stream->contract_apis.size(); ++i) {
        int new_flag = 1;
        
        for (int j = 0; j < stream->offline_apis.size(); ++j) {
            if (stream->contract_apis[i] == stream->offline_apis[j]) {
                new_flag = 0;
                continue;
            }
        }
        
        if (new_flag) {
            p_new_stream->contract_apis.push_back(stream->contract_apis[i]);
        }
    }
    
    p_new_stream->offline_apis = stream->offline_apis;
    p_new_stream->contract_emit_events = stream->contract_emit_events;
    p_new_stream->contract_storage_properties = stream->contract_storage_properties;
    p_new_stream->contract_id = stream->contract_id;
    p_new_stream->contract_name = stream->contract_name;
    p_new_stream->contract_level = stream->contract_level;
    p_new_stream->contract_state = stream->contract_state;
    FILE *f = fopen(name.c_str(), "wb");
    
    if (NULL == f) {
        delete (p_new_stream);
        strcpy(err_msg, strerror(errno));
        return -1;
    }
    
    sha.process_bytes(p_new_stream->buff.data(), p_new_stream->buff.size());
    sha.get_digest(digest);
    
    for (int i = 0; i < 5; ++i)
        lvm::utilities::common_fwrite_int(f, (int*)&digest[i]);
        
    int p_new_stream_buf_size = (int)p_new_stream->buff.size();
    lvm::utilities::common_fwrite_int(f, &p_new_stream_buf_size);
    p_new_stream->buff.resize(p_new_stream_buf_size);
    lvm::utilities::common_fwrite_stream(f, p_new_stream->buff.data(), p_new_stream->buff.size());
    int contract_apis_count = (int)p_new_stream->contract_apis.size();
    lvm::utilities::common_fwrite_int(f, &contract_apis_count);
    
    for (int i = 0; i < contract_apis_count; ++i) {
        int api_len = p_new_stream->contract_apis[i].length();
        lvm::utilities::common_fwrite_int(f, &api_len);
        lvm::utilities::common_fwrite_stream(f, p_new_stream->contract_apis[i].c_str(), api_len);
    }
    
    int offline_apis_count = (int)p_new_stream->offline_apis.size();
    lvm::utilities::common_fwrite_int(f, &offline_apis_count);
    
    for (int i = 0; i < offline_apis_count; ++i) {
        int offline_api_len = p_new_stream->offline_apis[i].length();
        lvm::utilities::common_fwrite_int(f, &offline_api_len);
        lvm::utilities::common_fwrite_stream(f, p_new_stream->offline_apis[i].c_str(), offline_api_len);
    }
    
    int contract_emit_events_count = p_new_stream->contract_emit_events.size();
    lvm::utilities::common_fwrite_int(f, &contract_emit_events_count);
    
    for (int i = 0; i < contract_emit_events_count; ++i) {
        int event_len = p_new_stream->contract_emit_events[i].length();
        lvm::utilities::common_fwrite_int(f, &event_len);
        lvm::utilities::common_fwrite_stream(f, p_new_stream->contract_emit_events[i].c_str(), event_len);
    }
    
    int contract_storage_properties_count = p_new_stream->contract_storage_properties.size();
    lvm::utilities::common_fwrite_int(f, &contract_storage_properties_count);
    
    for (const auto& storage_info : p_new_stream->contract_storage_properties) {
        int storage_len = storage_info.first.length();
        lvm::utilities::common_fwrite_int(f, &storage_len);
        lvm::utilities::common_fwrite_stream(f, storage_info.first.c_str(), storage_len);
        int storage_type = storage_info.second;
        lvm::utilities::common_fwrite_int(f, &storage_type);
    }
    
    fclose(f);
    delete (p_new_stream);
    return 0;
}
