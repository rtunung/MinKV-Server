#include <iostream>

#include "min_kv_server.h"
#include "config.h"

int main(int argc, char *argv[]) {

    std::string config_file = "config.cfg";
    if (argc >= 2) {
        config_file = argv[1];
    }

    min_kv::config cfg(config_file);
    min_kv::min_kv_server server(cfg);

    std::cout << "INFO: Starting MinKV Server" << std::endl;
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}