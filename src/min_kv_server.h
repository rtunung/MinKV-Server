#ifndef KV_SERVER_KV_SERVER_H
#define KV_SERVER_KV_SERVER_H

#include <unordered_map>

#include <asio.hpp>

#include "rr_server.h"
#include "parser.h"

namespace min_kv {

    class min_kv_server {
    public:

        // Non-SSL Version
        explicit min_kv_server(unsigned int port);

        // SSL Version
        min_kv_server(unsigned int port, const std::string& cert_file,const std::string& key_file);

        void run();
        void insert_key_value(const std::string& ky, std::string value);
        std::string get_value(const std::string& ky);
        void delete_key_value(const std::string& ky);

    private:
        handler build_handler();

        std::unordered_map<std::string, std::string> key_value_storage_;
        std::unique_ptr<rr_server> server_;
        asio::io_context io_;
        handler handler_;
    };
} // kv_server

#endif //KV_SERVER_KV_SERVER_H