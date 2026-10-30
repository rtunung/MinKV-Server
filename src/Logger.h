#ifndef MINKV_LOGGER_H
#define MINKV_LOGGER_H
#include <memory>
#include <spdlog/async.h>

namespace min_kv {
    class Logger {
    public:
        static void init();
        static void add_file_sink(const std::string &file_path);

    };
} // min_kv

#endif //MINKV_LOGGER_H
