
#include <base/exceptions.hpp>
#include <cli/cli.hpp>
#include <client/client.hpp>
#include <task/task.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <fc/io/console.hpp>
#include <fc/io/sstream.hpp>

#include <iostream>
#include <memory>

Cli::Cli(Client* client)
    : _p_input_stream(nullptr),
      _cin_thread("stdin_reader"),
      _p_out_stream(&std::cout),
      _p_client(client),
      _b_quit(false),
      _method_data_handler(std::make_shared<MethodDataHandler>()) {
}

Cli::~Cli() {
}

std::string Cli::get_line(const std::string& prompt, bool no_echo) {
    if (_b_quit) {
        return std::string();
    }

    return get_line(
        _p_input_stream,
        _p_out_stream,
        prompt,
        no_echo,
        &_cin_thread);
    
}

FC_DECLARE_EXCEPTION(cli_exception, 11000, "CLI Error")
    FC_DECLARE_DERIVED_EXCEPTION(abort_cli_command, cli_exception, 11001, "command aborted by user");
FC_DECLARE_DERIVED_EXCEPTION(exit_cli_command, cli_exception, 11002, "exit CLI client requested by user");

std::string Cli::get_line(
    std::istream* input_stream,
    std::ostream* out,
    const std::string& prompt,
    bool no_echo,     
    fc::thread* cin_thread) {
    if (input_stream == nullptr)
        FC_CAPTURE_AND_THROW(exit_cli_command); //_input_stream != nullptr );

    //FC_ASSERT( _self->is_interactive() );
    std::string line;
    if (no_echo) {
        *out << prompt;
        // there is no need to add input to history when echo is off, so both Windows and Unix implementations are same
        fc::set_console_echo(false);
        sync_call(cin_thread, [&](){ std::getline(*input_stream, line); }, "getline");
        fc::set_console_echo(true);
        *out << std::endl;
    } else {
        *out << prompt;
        sync_call(cin_thread, [&](){ std::getline(*input_stream, line); }, "getline");
    }

    boost::trim(line);
    return line;
}

std::string Cli::get_prompt() {
    std::string prompt_name = LVM_NMAE;
    std::string prompt_postfix = LVM_CLI_PROMPT_SUFFIX;
    std::string prompt = prompt_name + prompt_postfix;

    return prompt;
}

void Cli::parse_and_execute_interactive_command(std::string command, fc::istream_ptr argument_stream) {
    if (command.empty()) {
        return;
    }

    if (command == "quit" || command == "stop" || command == "exit") { 
        _p_client->quit_client(true);
        FC_THROW_EXCEPTION(exit_cli_command, "quit command issued");
    }

    BufferedIstreamWithEotHack buffered_argument_stream(argument_stream);

    bool command_is_valid = false;
    fc::variants arguments;
    try {
            MethodData* method_data = _method_data_handler->find_method_data(command);
            if (method_data) {
                handle_task(command, &buffered_argument_stream);
            }
        // NOTE: arguments here have not been filtered for private keys or passwords
        // ilog( "command: ${c} ${a}", ("c",command)("a",arguments) );
        command_is_valid = true;
    } catch (const lvm::global_exception::unknown_method&) {
        if (!command.empty()){
            *_p_out_stream << "Error: invalid command \"" << command << "\"\n";
        }
    } catch (const abort_cli_command&) {
        throw;
    } catch (const fc::exception& e) {
        *_p_out_stream << e.to_detail_string() << "\n";
        *_p_out_stream << "Error parsing command \"" << command << "\": " << e.to_string() << "\n";
        arguments = fc::variants{ command };
        edump((e.to_detail_string()));
    }
} //parse_and_execute_interactive_command

bool Cli::execute_command_line(const std::string& line, std::ostream* output/* = nullptr*/) {
    try {
        std::string trimmed_line_to_parse(boost::algorithm::trim_copy(line));
        /**
        *  On some OS X systems, std::stringstream gets corrupted and does not throw eof
        *  when expected while parsing the command.  Adding EOF (0x04) characater at the
        *  end of the string casues the JSON parser to recognize the EOF rather than relying
        *  on stringstream.
        *
        *  @todo figure out how to fix things on these OS X systems.
        */
        trimmed_line_to_parse += std::string(" ") + char(0x04);
        if (!trimmed_line_to_parse.empty()) {
            std::string::const_iterator iter = std::find_if(trimmed_line_to_parse.begin(), trimmed_line_to_parse.end(), ::isspace);
            std::string command;
            fc::istream_ptr argument_stream;
            if (iter != trimmed_line_to_parse.end()) {
                // then there are arguments to this function
                size_t first_space_pos = iter - trimmed_line_to_parse.begin();
                command = trimmed_line_to_parse.substr(0, first_space_pos);
                argument_stream = std::make_shared<fc::stringstream>((trimmed_line_to_parse.substr(first_space_pos + 1)));
            } else {
                command = trimmed_line_to_parse;
                argument_stream = std::make_shared<fc::stringstream>();
            }
            try {
                parse_and_execute_interactive_command(command, argument_stream);
            } catch (const exit_cli_command&) {
                return false;
            } catch (const abort_cli_command&) {
                *_p_out_stream << "Command aborted\n";
            }
        } //end if command line not empty
        return true;
    } FC_RETHROW_EXCEPTIONS(warn, "", ("command", line))
}

void Cli::process_commands(std::istream* input_stream) {
    try {
        FC_ASSERT(input_stream != nullptr);
        _p_input_stream = input_stream;
        //force flushing to console and log file whenever input is read
        _p_input_stream->tie(_p_out_stream);
        std::string line = get_line(get_prompt());
        while (_p_input_stream->good() && !_b_quit) {
            if (!execute_command_line(line))
                break;
            if (!_b_quit)
                line = get_line(get_prompt());
        } // while cin.good
        wlog("process commands exiting");
    }  FC_CAPTURE_AND_RETHROW()
}

void Cli::start() {
    try {
        if (!_b_quit) {
            process_commands(&std::cin);
        }
    } catch (const fc::exception& e) {
        *_p_out_stream << "\nshutting down\n";
        elog("${e}", ("e", e.to_detail_string()));
    }
}

TaskBase* Cli::parse_to_task(const std::string& task,
    fc::buffered_istream* argument_stream) {
    MethodData* method_data = _method_data_handler->find_method_data(task);
    if (!method_data) {
        return nullptr;
    }

    TaskBase* task_base = gen_task_base_from_method_data(method_data,
        argument_stream);
    task_base->task_from = FROM_CLI;

    return task_base;
}

void Cli::task_finished(TaskImplResult* result) {
    format_and_print_result(result);
}

void Cli::format_and_print_result(TaskImplResult* result) {
    std::string str_result = "Internal error.";
    if (result) {
        str_result = result->get_result_string();
    }

    *_p_out_stream << "result : \n" <<str_result << "\n";
}
