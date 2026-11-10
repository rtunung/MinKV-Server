#include "logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace min_kv {
    static std::shared_ptr<spdlog::async_logger> logger_;

    void logger::init() {
        spdlog::init_thread_pool(8192, 1);

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        std::vector<spdlog::sink_ptr> sinks {console_sink};

        logger_ = std::make_shared<spdlog::async_logger>(
            "MinKV",
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
        );

        logger_->set_level(spdlog::level::debug);

        spdlog::register_logger(logger_);
        spdlog::set_default_logger(logger_);
    }

    void logger::add_file_sink(const std::string &file_path) {
        auto rotating_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            file_path,
            10 * 1024 * 1024,
            5);
        logger_->sinks().emplace_back(rotating_file_sink);
    }
} // min_kv