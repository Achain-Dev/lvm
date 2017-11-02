#include <base/config.hpp>

#include <fc/log/file_appender.hpp>
#include <fc/reflect/variant.hpp>

#include <iostream>

fc::logging_config create_default_logging_config(const fc::path& data_dir, bool enable_ulog) {
    fc::logging_config cfg;
    fc::path log_dir("logs");

    fc::file_appender::config ac;
    ac.filename = log_dir / "default" / "default.log";
    ac.flush = true;
    ac.rotate = true;
    ac.rotation_interval = fc::hours(1);
    ac.rotation_limit = fc::days(1);

    std::cout << "Logging to file: " << (data_dir / ac.filename).preferred_string() << "\n";

    fc::variants  c{
        fc::mutable_variant_object("level", "debug")("color", "green"),
        fc::mutable_variant_object("level", "warn")("color", "brown"),
        fc::mutable_variant_object("level", "error")("color", "red") };

    cfg.appenders.push_back(
        fc::appender_config("stderr", "console",
        fc::mutable_variant_object()
        ("stream", "std_error")
        ("level_colors", c)
        ));

    cfg.appenders.push_back(fc::appender_config("default", "file", fc::variant(ac)));

    fc::logger_config dlc_user;
    if (enable_ulog) {
        dlc_user.level = fc::log_level::debug;
    } else {
        dlc_user.level = fc::log_level::off;
    }
    dlc_user.name = "user";
    dlc_user.appenders.push_back("user");

    cfg.loggers.push_back(dlc_user);

    return cfg;
}
