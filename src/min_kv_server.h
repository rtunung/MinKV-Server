#ifndef KV_SERVER_KV_SERVER_H
#define KV_SERVER_KV_SERVER_H

#include <asio.hpp>

#include "rr_server.h"
#include "config.h"
#include "key_value_storage.h"
#include "user_db_connection.h"

namespace min_kv {

    class min_kv_server {
    public:
        // Non-SSL Version
        [[deprecated]]
        explicit min_kv_server(unsigned int port);
        // SSL Version
        [[deprecated]]
        min_kv_server(unsigned int port, const std::string& cert_file,const std::string& key_file);

        // Config File Version
        explicit min_kv_server(config &conf);

        void run();

    private:
        handler_function build_handler();

        key_value_storage kv_storage;
        std::unique_ptr<rr_server> server;
        asio::io_context io;
        handler_function handler;

        int num_threads;

        bool authentication_required;
        std::unique_ptr<user_db_connection> user_db;
    };
} // kv_server

#endif //KV_SERVER_KV_SERVER_H