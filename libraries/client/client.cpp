#define DEFAULT_LOGGER "client"

#include <base/config.hpp>
#include <cli/cli.hpp>
#include <client/client.hpp>
#include <rpc/rpc_mgr.hpp>

#include <openssl/opensslv.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/reverse.hpp>
#include <boost/version.hpp>

#include <fc/crypto/base58.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/hex.hpp>
#include <fc/filesystem.hpp>
#include <fc/git_revision.hpp>
#include <fc/io/fstream.hpp>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/file_appender.hpp>
#include <fc/log/logger.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/network/http/connection.hpp>
#include <fc/network/resolve.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/thread/non_preemptable_scope_check.hpp>
#include <fc/thread/thread.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>

using namespace boost;
using std::string;

boost::program_options::variables_map Client::parse_option_variables(int argc, char** argv) {
    // parse command-line options
    program_options::options_description option_config("Usage");
    option_config.add_options()
    ("debug-mode", program_options::value<bool>()->default_value(false), "close debug mode by default")
    ("data-dir", program_options::value<std::string>(), "Set lvm data directory")
    ;
    program_options::variables_map option_variables;
    
    try {
        program_options::store(program_options::command_line_parser(argc, argv).
                               options(option_config).run(), option_variables);
        program_options::notify(option_variables);
        
    } catch (program_options::error& cmdline_error) {
        std::cerr << "Error: " << cmdline_error.what() << "\n";
        std::cerr << option_config << "\n";
        exit(1);
    }
    
    return option_variables;
}

fc::path Client::get_data_dir(const program_options::variables_map& option_variables) {
    try {
        fc::path datadir;
        
        if (option_variables.count("data-dir")) {
#ifdef WIN32
            datadir = fc::path(option_variables["data-dir"].as<string>());
#else
            datadir = fc::path(option_variables["data-dir"].as<string>().c_str());
#endif
            
        } else {
            const auto get_os_specific_dir_name = [&](string dir_name) -> string {
#ifdef WIN32
#elif defined( __APPLE__ )
#else
                std::string::iterator end_pos = std::remove(dir_name.begin(), dir_name.end(), ' ');
                dir_name.erase(end_pos, dir_name.end());
                dir_name = "." + dir_name;
#endif
            
                return dir_name;
            };
            datadir = fc::app_path() / get_os_specific_dir_name(LVM_NMAE);
        }
        
        return datadir;
    }
    
    FC_RETHROW_EXCEPTIONS(warn, "error loading config")
}

void Client::configure_from_command_line(int argc, char** argv) {
    if (argc == 0 && argv == nullptr) {
        //my->_cli = new thinkyoung::cli::Cli(this, nullptr, &std::cout);
        return;
    }
    
    // parse command-line options
    auto option_variables = parse_option_variables(argc, argv);
    fc::path datadir = get_data_dir(option_variables);
    _ob_global_config.data_file_path = datadir;
    
    if (!fc::exists(_ob_global_config.data_file_path)) {
        std::cout << "lvm Creating new data directory " << _ob_global_config.data_file_path.preferred_string() << "\n";
        fc::create_directories(_ob_global_config.data_file_path);
#ifndef WIN32
        int perm = 0700;
        std::cout << "Setting UNIX permissions on new data directory to " << std::oct << perm << std::dec << "\n";
        fc::chmod(datadir, perm);
#else
        std::cout << "Note: new data directory is readable by all user accounts on non-UNIX OS\n";
#endif
    }
    
    _ob_global_config.logging = create_default_logging_config(datadir, _enable_ulog);
    fc::configure_logging(_ob_global_config.logging);
    init();
} //configure_from_command_line

Client::Client()
    : _b_client_quit(false),
      _enable_ulog(false) {
}

Client::~Client() {
}

void Client::init() {
    if (!_sp_cli) {
        _sp_cli = std::make_shared<Cli>(this);
    }
    
    if (!_sp_rpc_mgr) {
        _sp_rpc_mgr = std::make_shared<RpcMgr>(this);
        _sp_rpc_mgr->set_endpoint(std::string("127.0.0.1"), 65000, ASYNC_MODE);
        _sp_rpc_mgr->set_endpoint(std::string("127.0.0.1"), 65001, SYNC_MODE);
        _sp_rpc_mgr->start();
    }
}

fc::future<void> Client::start() {
    _client_done = fc::async([=]() {
        _sp_cli->start();
    }, "Client::start");
    return _client_done;
}