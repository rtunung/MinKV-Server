#ifndef KV_SERVER_KV_SERVER_H
#define KV_SERVER_KV_SERVER_H

#include <unordered_map>

#include <asio.hpp>

#include "request_response_server.h"
#include "parser.h"

namespace min_kv {

    class min_kv_server {
    public:
        explicit min_kv_server(unsigned int port);

        void run();
        void insert_key_value(const std::string& ky, std::string value);
        std::string get_value(const std::string& ky);

    private:
        handler build_handler();

        std::unordered_map<std::string, std::string> key_value_storage_;
        std::unique_ptr<request_response_server> server_;
        asio::io_context io_;
        handler handler_;
    };
} // kv_server

#endif //KV_SERVER_KV_SERVER_H