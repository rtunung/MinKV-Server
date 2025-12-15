#ifndef KV_SERVER_SERVER_H
#define KV_SERVER_SERVER_H

#include <asio/ip/tcp.hpp>
#include <functional>

#include "session.h"

namespace min_kv {

    class request_response_server {
    public:
        request_response_server(asio::io_context &context, unsigned short port,
            handler h);

    private:
        void accept();

        asio::ip::tcp::acceptor acceptor_;
        handler handler_;
    };
} // kv_server

#endif //KV_SERVER_SERVER_H