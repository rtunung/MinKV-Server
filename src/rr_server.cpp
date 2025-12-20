#include "rr_server.h"

#include <iostream>
#include <asio.hpp>
#include <utility>

#include "ssl_session.h"

namespace min_kv {

    rr_server::rr_server(asio::io_context &context, unsigned short port, handler_function h) :
            acceptor(context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),handler(std::move(h)) {
    }

    void rr_server::accept() {
        acceptor.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::cout << "INFO: Opening a new TCP session." << std::endl;
                    std::make_shared<session>(std::move(socket), this->handler)->start();
                }
                else
                    std::cerr << "ERROR: could not open session: " << ec.message() << std::endl;

                accept();
            });
    }
} // kv_server