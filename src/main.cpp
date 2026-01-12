#include <iostream>

#define ASIO_STANDALONE
#include "min_kv_server.h"
#include "config.h"

int main(int argc, char *argv[]) {

    if (argc >= 5) {
        if (std::strcmp(argv[1], "add-user") == 0) {
            std::string database = argv[2];
            std::string user = argv[3];
            std::string password = argv[4];
            min_kv::salt_t salt = {0};
            min_kv::hash_t hash = {0};
            min_kv::hash_scrypt(password, salt, hash);

            min_kv::user_db_connection db(database);
            db.create_user({user, true, true, hash, salt});
            OPENSSL_cleanse(password.data(), password.size());
            OPENSSL_cleanse(hash.data(), hash.size());
            OPENSSL_cleanse(salt.data(), salt.size());

            std::cout << "Added user " << user << std::endl;
            return 0;
        }
    }

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