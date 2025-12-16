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
            handler h);

    protected:
        virtual void accept();

        asio::ip::tcp::acceptor acceptor_;
        handler handler_;
    };
} // kv_server

#endif //KV_SERVER_SERVER_H