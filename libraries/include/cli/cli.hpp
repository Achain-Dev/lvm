/*
    author: pli
    date: 2017.10.17
    The command line handler.
    */

#ifndef _CLI_H_
#define _CLI_H_

#include <base/config.hpp>
#include <cli/method_data_handler.hpp>
#include <task/task_handler_base.hpp>

#include <boost/program_options.hpp>

#include <fc/filesystem.hpp>
#include <fc/io/buffered_iostream.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/thread/thread.hpp>
#include <fc/variant.hpp>

#include <memory>

class Client;

class BufferedIstreamWithEotHack : public virtual fc::buffered_istream {
public:
    BufferedIstreamWithEotHack(fc::istream_ptr is) :
        fc::buffered_istream(is) {}
    BufferedIstreamWithEotHack(fc::buffered_istream&& o) :
        fc::buffered_istream(std::move(o)) {}

    std::size_t readsome(char* buf, std::size_t len) override {
        std::size_t bytes_read = fc::buffered_istream::readsome(buf, len);
        assert(bytes_read > 0);
        if (buf[bytes_read - 1] == 0x04)
            --bytes_read;
        if (bytes_read == 0)
            FC_THROW_EXCEPTION(fc::eof_exception, "");
        return bytes_read;
    }

    size_t readsome(const std::shared_ptr<char>& buf, size_t len, size_t offset) override {
        std::size_t bytes_read = fc::buffered_istream::readsome(buf, len, offset);
        assert(bytes_read > 0);
        if (buf.get()[offset + bytes_read - 1] == 0x04)
            --bytes_read;
        if (bytes_read == 0)
            FC_THROW_EXCEPTION(fc::eof_exception, "");
        return bytes_read;
    }

    char peek() const override {
        char c = fc::buffered_istream::peek();
        if (c == 0x04)
            FC_THROW_EXCEPTION(fc::eof_exception, "");
        return c;
    }

    char get() override {
        char c = fc::buffered_istream::get();
        if (c == 0x04)
            FC_THROW_EXCEPTION(fc::eof_exception, "");
        return c;
    }
};

class Cli : public TaskHandlerBase {
public:
    Cli(Client* client);
    virtual ~Cli();

    void start();

    // override from TaskHandlerBase
protected:
    virtual void task_finished(TaskImplResult* result);
    virtual TaskBase* parse_to_task(const std::string& task,
        fc::buffered_istream* argument_stream);

private:
    void parse_and_execute_interactive_command(std::string command,
        fc::istream_ptr argument_stream);
    bool execute_command_line(const std::string& line, std::ostream* output = nullptr);
    void process_commands(std::istream* input_stream);
    void format_and_print_result(TaskImplResult* result);

    std::string get_prompt();
    std::string get_line(const std::string& prompt = LVM_CLI_PROMPT_SUFFIX, bool no_echo = false);
    std::string get_line(std::istream* input_stream,
        std::ostream* out,
        const std::string& prompt,
        bool no_echo,
        fc::thread* cin_thread);

private:
    std::istream*     _p_input_stream;
    std::ostream*     _p_out_stream;
    Client*			  _p_client;
    fc::thread        _cin_thread;
    bool              _b_quit;

private:
    MethodDataHandlerPtr _method_data_handler;
};

#endif
