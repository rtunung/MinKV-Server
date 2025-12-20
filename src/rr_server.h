#ifndef KV_SERVER_SERVER_H
#define KV_SERVER_SERVER_H

#include <asio/ip/tcp.hpp>
#include <functional>

#include "session.h"

namespace min_kv {

    class rr_server {
    public:
        virtual ~rr_server() = default;

        // Non-SSL Version
        rr_server(asio::io_context &context, unsigned short port,
            handler_function h);

        virtual void accept();

    protected:
        asio::ip::tcp::acceptor acceptor;
        handler_function handler;
    };
} // kv_server

#endif //KV_SERVER_SERVER_H