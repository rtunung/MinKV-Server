
#include "ssl_rr_server.h"

#include <iostream>

#include "ssl_session.h"

namespace min_kv {
    ssl_rr_server::ssl_rr_server(asio::io_context &context, unsigned short port, const std::string &cert_file,
        const std::string &key_file, handler h) : rr_server(context,port,h),
        ssl_ctx_(asio::ssl::context::tls_server)  {

        ssl_ctx_.set_options(
            asio::ssl::context::default_workarounds |
            asio::ssl::context::no_sslv2 |
            asio::ssl::context::no_sslv3 |
            asio::ssl::context::single_dh_use);

        ssl_ctx_.use_certificate_chain_file(cert_file);
        ssl_ctx_.use_private_key_file(key_file, asio::ssl::context::pem);

        ssl_rr_server::accept();
    }

    void ssl_rr_server::accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::cout << "INFO: Opening a new TLS session." << std::endl;
                    std::make_shared<ssl_session>(std::move(socket), ssl_ctx_, this->handler_)->start();
                }
                else
                    std::cerr << "ERROR: could not open session: " << ec.message() << std::endl;

                accept();
            });
    }
} // min_kv