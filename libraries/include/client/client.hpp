/*
    author: pli
    date: 2017.10.17
    The Client entry class.
*/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <base/config.hpp>

#include <boost/program_options.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/thread/thread.hpp>
#include <fc/filesystem.hpp>

#include <memory>

class Cli;
class RpcMgr;


class Client {
  public:
    Client();
    virtual ~Client();
    
    Config  get_global_config() {
        return _ob_global_config;
    };
    bool is_client_quit() {
        return _b_client_quit;
    };
    void quit_client(bool quit) {
        _b_client_quit = quit;
    };
    
    void configure_from_command_line(int argc, char** argv);
    fc::future<void> start();
    
  private:
    boost::program_options::variables_map parse_option_variables(int argc,
            char** argv);
    fc::path get_data_dir(
        const boost::program_options::variables_map& option_variables);
    void    init();
    
  private:
    Config  _ob_global_config;
    fc::future<void>  _client_done;
    bool              _b_client_quit;
    bool              _enable_ulog;
    std::shared_ptr<Cli>    _sp_cli;
    std::shared_ptr<RpcMgr> _sp_rpc_mgr;
};

typedef std::shared_ptr<Client> ClientPtr;

#endif
