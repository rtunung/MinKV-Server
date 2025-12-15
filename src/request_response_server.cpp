#include "request_response_server.h"

#include <iostream>
#include <asio.hpp>
#include <utility>

namespace min_kv {

    request_response_server::request_response_server(asio::io_context &context, unsigned short port,
                                               handler h) :
            acceptor_(context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),handler_(std::move(h)) {
        accept();
    }

    void request_response_server::accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::cout << "INFO: Opening a new session." << std::endl;
                    std::make_shared<session>(std::move(socket), this->handler_)->start();
                }
                else
                    std::cout << ec.message() << std::endl;

                accept();
            });
    }
} // kv_server