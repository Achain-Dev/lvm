#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <fc/filesystem.hpp>
#include <fc/log/logger_config.hpp>

#define LVM_NMAE   "LVM"
#define LVM_CLI_PROMPT_SUFFIX  ">>> "
#define DISPATCH_TASK_TIMESPAN  1
#define SEND_HELLO_MSG_INTERVAL 30
#define INVALIDE_SUB_ADDRESS ("ffffffffffffffffffffffffffffffff")
#define ACT_ADDRESS_PREFIX "ACT"
#define ALP_ADDRESS_PREFIX ACT_ADDRESS_PREFIX

struct Config {
    bool                debug_mode;
    fc::path            data_file_path;
    fc::logging_config  logging;
};

fc::logging_config create_default_logging_config(const fc::path& data_dir, bool enable_ulog);

#endif
