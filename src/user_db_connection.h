
#ifndef MINKV_USER_DB_CONNECTION_H
#define MINKV_USER_DB_CONNECTION_H

#include <string>
#include <sstream>
#include <memory>

#include <sqlite3.h>

namespace min_kv {
struct user {
    std::string name;
    bool read_permission;
    bool write_permission;
    std::string password;
};

class user_db_connection {
    public:
    explicit user_db_connection(const std::string &db_path);
    ~user_db_connection();

    void create_user(const user &u);
    std::unique_ptr<user> get_user(const std::string &name);

    private:
    sqlite3* db{};

    static int callback_get_user(void*, int argc, char** argv, char** colName);
    static std::unique_ptr<user> user_result;
};
} // min_kv

#endif //MINKV_USER_DB_CONNECTION_H