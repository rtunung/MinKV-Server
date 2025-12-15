#include "request_response_server.h"

#include <iostream>
#include <asio.hpp>
#include <utility>

#include "ssl_session.h"

namespace min_kv {

    request_response_server::request_response_server(asio::io_context &context, unsigned short port, handler h) :
            acceptor_(context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),handler_(std::move(h)),
            use_ssl_(false), ssl_ctx_(asio::ssl::context::tls_server) {
        accept();
    }

    request_response_server::request_response_server(asio::io_context &context, unsigned short port,
        const std::string &cert_file, const std::string &key_file, handler h) :
            acceptor_(context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),handler_(std::move(h)),
            use_ssl_(true), ssl_ctx_(asio::ssl::context::tls_server){

        ssl_ctx_.set_options(
            asio::ssl::context::default_workarounds |
            asio::ssl::context::no_sslv2 |
            asio::ssl::context::no_sslv3 |
            asio::ssl::context::single_dh_use);

        ssl_ctx_.use_certificate_chain_file(cert_file);
        ssl_ctx_.use_private_key_file(key_file, asio::ssl::context::pem);

        accept();
    }

    void request_response_server::accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    if (use_ssl_) {
                        std::cout << "INFO: Opening a new TLS session." << std::endl;
                        std::make_shared<ssl_session>(std::move(socket), ssl_ctx_, this->handler_)->start();
                        //std::make_shared<session>(std::move(socket), ssl_ctx_, this->handler_)->start();
                    } else {
                        std::cout << "INFO: Opening a new TCP session." << std::endl;
                        std::make_shared<session>(std::move(socket), this->handler_)->start();
                    }
                }
                else
                    std::cerr << "ERROR: could not open session: " << ec.message() << std::endl;

                accept();
            });
    }
} // kv_server