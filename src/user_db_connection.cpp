#include "user_db_connection.h"

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
            "password TEXT NOT NULL);";

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

    void user_db_connection::create_user(const user &u) {
        std::stringstream sql_cmd;
        sql_cmd << "INSERT INTO users (name, read_permission, write_permission, password) VALUES (";
        sql_cmd << "'" << u.name << "', ";
        sql_cmd << u.read_permission << ", ";
        sql_cmd << u.write_permission << ", ";
        sql_cmd << "'" << u.password << "');";

        char* err_msg = nullptr;
        int rc;

        rc = sqlite3_exec(db, sql_cmd.str().c_str(), nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::string err_str = "Can't create user: " + u.name + " \nSQLite error:  " + err_msg;
            sqlite3_free(err_msg);
            throw std::runtime_error(err_str);
        }
    }

    std::unique_ptr<user> user_db_connection::get_user(const std::string &name) {
        std::string sql_cmd = "SELECT * FROM users WHERE name ='" + name + "';";
        char* err_msg = nullptr;
        int rc;
        rc = sqlite3_exec(db, sql_cmd.c_str(), callback_get_user, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::string err_str = "Could not find user: " + name + " \nSQLite error:  " + err_msg;
            sqlite3_free(err_msg);
            throw std::runtime_error(err_str);
        }

        return std::move(user_result);
    }

    int user_db_connection::callback_get_user(void *, int argc, char **argv, char **colName) {
        user tmp;
        tmp.name = argv[1];
        tmp.read_permission = std::atoi(argv[2]);
        tmp.write_permission = std::atoi(argv[3]);
        tmp.password = argv[4];

        user_result = std::make_unique<user>(std::move(tmp));

        return 0;
    }
} // min_kv