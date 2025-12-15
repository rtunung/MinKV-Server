#ifndef KV_SERVER_SERVER_H
#define KV_SERVER_SERVER_H

#include <asio/ip/tcp.hpp>
#include <functional>

#include "session.h"

namespace min_kv {

    class request_response_server {
    public:

        // Non-SSL Version
        request_response_server(asio::io_context &context, unsigned short port,
            handler h);

        // SSL Version
        request_response_server(asio::io_context& context, unsigned short port,
            const std::string& cert_file,const std::string& key_file, handler h);

    private:
        void accept();

        asio::ip::tcp::acceptor acceptor_;
        handler handler_;
        asio::ssl::context ssl_ctx_;

        const bool use_ssl_;
    };
} // kv_server

#endif //KV_SERVER_SERVER_H