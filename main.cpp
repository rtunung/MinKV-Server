#include <iostream>

#include "min_kv_server.h"

int main() {

    min_kv::min_kv_server server(5555);
    std::cout << "Staring MinKV Server" << std::endl;
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}