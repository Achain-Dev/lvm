#include <client/client.hpp>
#include <stub/stub.hpp>

#include <fc/log/logger_config.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>

bool g_stub_test_enable = false;

void rolloutHandler(const char* filename, std::size_t size) {
    static int idx = 0;
    std::stringstream stream;
    stream << filename << "." << ++idx << ".log";
    fc::rename(filename, stream.str().c_str());
}

void log_init() {
}

void log_close() {
}

int main(int argc, char** argv) {
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
