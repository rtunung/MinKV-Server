
#include "ssl_rr_server.h"

#include <iostream>
#include <spdlog/spdlog.h>

#include "ssl_session.h"

namespace min_kv {
    ssl_rr_server::ssl_rr_server(asio::io_context &context, unsigned short port, const std::string &cert_file,
        const std::string &key_file, handler_function h) : rr_server(context,port,h),
        ssl_ctx(asio::ssl::context::tls_server)  {

        ssl_ctx.set_options(
            asio::ssl::context::default_workarounds |
            asio::ssl::context::no_sslv2 |
            asio::ssl::context::no_sslv3 |
            asio::ssl::context::single_dh_use);

        ssl_ctx.use_certificate_chain_file(cert_file);
        ssl_ctx.use_private_key_file(key_file, asio::ssl::context::pem);
    }

    void ssl_rr_server::accept() {
        acceptor.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                const auto address = socket.remote_endpoint().address().to_string();
                if (!ec) {
                    spdlog::info("Opening new TLS session with client {}", address);
                    std::make_shared<ssl_session>(std::move(socket), ssl_ctx, this->handler)->start();
                }
                else
                    spdlog::error("Could not open TLS session with client {}. {}", address, ec.message());

                accept();
            });
    }
} // min_kv