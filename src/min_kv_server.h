#ifndef KV_SERVER_KV_SERVER_H
#define KV_SERVER_KV_SERVER_H

#include <asio.hpp>

#include "rr_server.h"
#include "parser.h"
#include "key_value_storage.h"

namespace min_kv {

    class min_kv_server {
    public:

        // Non-SSL Version
        explicit min_kv_server(unsigned int port);

        // SSL Version
        min_kv_server(unsigned int port, const std::string& cert_file,const std::string& key_file);

        void run(std::size_t num_threads);

    private:
        handler_function build_handler();

        key_value_storage kv_storage;
        std::unique_ptr<rr_server> server;
        asio::io_context io;
        handler_function handler;
    };
} // kv_server

#endif //KV_SERVER_KV_SERVER_H