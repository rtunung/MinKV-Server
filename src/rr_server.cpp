#include "rr_server.h"

#include <iostream>
#include <asio.hpp>
#include <utility>

#include "ssl_session.h"

namespace min_kv {

    rr_server::rr_server(asio::io_context &context, unsigned short port, handler h) :
            acceptor_(context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),handler_(std::move(h)) {
        rr_server::accept();
    }

    void rr_server::accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::cout << "INFO: Opening a new TCP session." << std::endl;
                    std::make_shared<session>(std::move(socket), this->handler_)->start();
                }
                else
                    std::cerr << "ERROR: could not open session: " << ec.message() << std::endl;

                accept();
            });
    }
} // kv_server