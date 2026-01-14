#include <iostream>

#define ASIO_STANDALONE
#include "min_kv_server.h"
#include "config.h"

int main(int argc, char *argv[]) {

    auto print_usage = [argv]() {
        std::cout << "Usage:\n";
        std::cout << argv[0] << " <config-file>                                    -- Launch with specified config file\n";
        std::cout << argv[0] << " add-user <sqlite-database> <username> <password> -- Create new user inside the specified database\n";
    };

    if (argc >= 2) {
        if (std::strcmp(argv[1], "help") == 0)
            print_usage();
    }

    if (argc == 5) {
        if (std::strcmp(argv[1], "add-user") == 0) {
            std::string database = argv[2];
            std::string user = argv[3];
            std::string password = argv[4];
            min_kv::salt_t salt = {0};
            min_kv::hash_t hash = {0};
            min_kv::hash_scrypt(password, salt, hash);

            min_kv::user_db_connection db(database);
            min_kv::user usr = {user, true, true, hash, salt};
            db.create_user(usr);

            std::cout << "Added user " << user << std::endl;
            return 0;
        }
        print_usage();
    }

    std::string config_file = "config.cfg";
    if (argc == 2) {
        config_file = argv[1];
    }

    try {
        min_kv::config cfg(config_file);
        min_kv::min_kv_server server(cfg);
        std::cout << "INFO: Starting MinKV Server" << std::endl;
        server.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}