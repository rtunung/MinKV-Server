#include "user_db_connection.h"

#include <cstring>
#include <format>

#include <openssl/crypto.h>

namespace min_kv {
    std::unique_ptr<user> user_db_connection::user_result;

    user_db_connection::user_db_connection(const std::string &db_path) {
        int rc = sqlite3_open(db_path.c_str(), &db);
        if (rc) {
            std::string db_err = "Can't open database: " + db_path + " \n SQLite error: " + sqlite3_errmsg(db);
            throw std::runtime_error(db_err);
        }

        std::string create_table =
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL UNIQUE,"
            "read_permission INTEGER NOT NULL,"
            "write_permission INTEGER NOT NULL,"
            "password_hash TEXT NOT NULL);";

        char* err_msg = nullptr;
        rc = sqlite3_exec(db, create_table.c_str(), nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::string err_str = "Can't create table: " + db_path + " \nSQLite error: " + err_msg;
            sqlite3_free(err_msg);
            throw std::runtime_error(err_str);
        }
    }

    user_db_connection::~user_db_connection() {
        sqlite3_close(db);
    }

    void user_db_connection::create_user(user &u) {
        const std::lock_guard<std::mutex> lock(mutex);
        combined_hash_t combined_hash = {};
        combine_hash_salt(u.password_hash, u.password_salt, combined_hash);
        std::string hash_base64;
        encode_base64(combined_hash, hash_base64);

        std::string sql_cmd = std::format(
            "INSERT INTO users (name, read_permission, write_permission, password_hash) VALUES ("
            "'{}', {}, {}, '{}');", u.name, u.read_permission, u.write_permission, hash_base64);
        OPENSSL_cleanse(hash_base64.data(), hash_base64.size());

        char* err_msg = nullptr;
        int rc = sqlite3_exec(db, sql_cmd.c_str(), nullptr, nullptr, &err_msg);
        OPENSSL_cleanse(sql_cmd.data(), sql_cmd.size());
        if (rc != SQLITE_OK) {
            std::string err_str = "Can't create user: " + u.name + " \nSQLite error:  " + err_msg;
            sqlite3_free(err_msg);
            throw std::runtime_error(err_str);
        }
    }

    std::unique_ptr<user> user_db_connection::get_user(const std::string &name) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::string sql_cmd = std::format("SELECT * FROM users WHERE name = '{}';", name);
        char* err_msg = nullptr;
        int rc = sqlite3_exec(db, sql_cmd.c_str(), callback_get_user, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::string err_str = "Could not find user: " + name + " \nSQLite error:  " + err_msg;
            sqlite3_free(err_msg);
            throw std::runtime_error(err_str);
        }

        return std::move(user_result);
    }

    int user_db_connection::callback_get_user(void *, int argc, char **argv, char **colName) {
        auto tmp = std::make_unique<user>();
        tmp->name = argv[1];
        tmp->read_permission = std::atoi(argv[2]);
        tmp->write_permission = std::atoi(argv[3]);
        std::string base64 = argv[4];
        combined_hash_t combined_hash = {0};
        decode_base64(base64, combined_hash);
        split_combined_hash(combined_hash, tmp->password_hash, tmp->password_salt);
        OPENSSL_cleanse(argv[4], std::strlen(argv[4]));

        user_result = std::move(tmp);
        return 0;
    }
} // min_kv