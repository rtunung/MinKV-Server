#include "user_db_connection.h"

#include <cstring>
#include <format>

#include <openssl/crypto.h>

namespace min_kv {

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

        std::string sql_string =
            "INSERT INTO users (name, read_permission, write_permission, password_hash) VALUES ("
            "?, ?, ?, ?);";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql_string.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, u.name.c_str(), -1, nullptr);
        sqlite3_bind_int(stmt, 2, u.read_permission);
        sqlite3_bind_int(stmt, 3, u.write_permission);
        sqlite3_bind_text(stmt, 4, hash_base64.c_str(), -1, nullptr);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        OPENSSL_cleanse(hash_base64.data(), hash_base64.size());
    }

    std::optional<user> user_db_connection::get_user(const std::string &name) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::string sql_string = "SELECT * FROM users WHERE name = ?;";

        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, sql_string.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, nullptr);

        auto result  = sqlite3_step(stmt);
        if (result == 101)
            return {};

        user found_user;
        found_user.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        found_user.read_permission = sqlite3_column_int(stmt, 2);
        found_user.write_permission = sqlite3_column_int(stmt, 3);

        std::string base64 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        combined_hash_t combined_hash = {0};
        decode_base64(base64, combined_hash);
        split_combined_hash(combined_hash, found_user.password_hash, found_user.password_salt);

        sqlite3_finalize(stmt);
        return found_user;
    }
} // min_kv