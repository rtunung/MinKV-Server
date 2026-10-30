#include <iostream>
#include <termios.h>

#include <spdlog/spdlog.h>
#include "Logger.h"

#define ASIO_STANDALONE
#include "min_kv_server.h"
#include "config.h"

static int add_user(min_kv::user_db_connection &db, const std::string &name) {
    int error_code = 0;
    termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    new_termios.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    std::string pass1, pass2;
    std::cout << "Enter password: ";
    std::getline(std::cin, pass1);
    std::cout << std::endl;
    std::cout << "Confirm password: ";
    std::getline(std::cin, pass2);
    std::cout << std::endl;

    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

    if (pass1 == pass2) {
        min_kv::user usr;
        usr.name = name;
        usr.read_permission = true;
        usr.write_permission = true;
        min_kv::hash_scrypt(pass1, usr.password_salt, usr.password_hash);
        try {
            db.create_user(usr);
        } catch (std::exception &e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
        std::cout << "Successfully created user." << std::endl;
    } else {
        std::cout << "Passwords do not match" << std::endl;
        error_code = -1;
    }

    OPENSSL_cleanse(pass1.data(), pass1.size());
    OPENSSL_cleanse(pass2.data(), pass2.size());

    return error_code;
}

int main(const int argc, char *argv[]) {

    // Setup Logger
    min_kv::Logger::init();

    auto print_usage = [argv]() -> int {
        std::cout << "Usage:\n";
        std::cout << argv[0] << " <config-file>                         -- Launch with specified config file\n";
        std::cout << argv[0] << " add-user <sqlite-database> <username> -- Create new user inside the specified database\n";
        return 0;
    };

    if (argc >= 2) {
        if (std::strcmp(argv[1], "help") == 0)
            return print_usage();
    }

    if (argc == 4) {
        if (std::strcmp(argv[1], "add-user") == 0) {
            min_kv::user_db_connection db(argv[2]);
            return add_user(db, argv[3]);
        }
        return print_usage();
    }

    std::string config_file = "config.cfg";
    if (argc == 2) {
        config_file = argv[1];
    }

    try {
        min_kv::config cfg(config_file);

        const auto log_file_path = cfg.getStringValue("log_file");
        if ( log_file_path && !log_file_path.value().empty())
            min_kv::Logger::add_file_sink(log_file_path.value());

        min_kv::min_kv_server server(cfg);
        spdlog::info("Starting MinKV Server");
        server.run();
    } catch (const std::exception& e) {
        spdlog::critical("An unrecoverable error occurred: {}", e.what());
    }

    spdlog::shutdown();

    return 0;
}