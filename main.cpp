#include <base/easylogging++.h>
#include <client/client.hpp>
#include <stub/stub.hpp>

#include <fc/log/logger_config.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>

INITIALIZE_EASYLOGGINGPP
bool g_stub_test_enable = false;

void rolloutHandler(const char* filename, std::size_t size) {
    static int idx = 0;
    std::stringstream stream;
    stream << filename << "." << ++idx << ".log";
    fc::rename(filename, stream.str().c_str());
}

void log_init() {
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    defaultConf.set(el::Level::Global, el::ConfigurationType::Format, "%datetime[%level][%loc](%func): %msg");
    el::Loggers::reconfigureLogger("default", defaultConf);
    el::Loggers::setLoggingLevel(el::Level::Info);
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize, "10485760");//10M
    el::Helpers::installPreRollOutCallback(rolloutHandler);
}

void log_close() {
    el::Loggers::setLoggingLevel(el::Level::Unknown);
}

int main(int argc, char** argv) {
    log_init();
    LOG(INFO) << "stub:" << g_stub_test_enable;
    
    if (g_stub_test_enable) {
        StubPtr stub = std::make_shared<Stub>();
        stub->start();
        return 0;
    }
    
    log_close();
    
    try {
        ClientPtr client = std::make_shared<Client>();
        client->configure_from_command_line(argc, argv);
        client->start().wait();
        
    } catch (const fc::exception& e) {
        std::cerr << "------------ error --------------\n"
                  << e.to_detail_string() << "\n";
        wlog("${e}", ("e", e.to_detail_string()));
    }
    
    fc::configure_logging(fc::logging_config::default_config());
    return 0;
}
