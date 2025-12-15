#include <iostream>

#include "min_kv_server.h"

int main() {

    const std::string cert_file = "server.crt";
    const std::string key_file = "server.key";

    min_kv::min_kv_server server(5555, cert_file, key_file);
    std::cout << "INFO: Starting MinKV Server" << std::endl;
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}