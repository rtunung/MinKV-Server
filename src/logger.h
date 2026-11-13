#ifndef MINKV_LOGGER_H
#define MINKV_LOGGER_H
#include <memory>
#include <spdlog/async.h>

namespace min_kv {
    class logger {
    public:
        static void init();
        static void add_file_sink(const std::string &file_path, int num_files, std::size_t max_size);
        static void set_debug();

    };
} // min_kv

#endif //MINKV_LOGGER_H
