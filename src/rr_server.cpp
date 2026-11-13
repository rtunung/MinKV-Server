#include "rr_server.h"

#include <asio.hpp>
#include <utility>

#include <spdlog/spdlog.h>

#include "ssl_session.h"

namespace min_kv {

    rr_server::rr_server(asio::io_context &context, unsigned short port, handler_function h) :
            acceptor(context,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),handler(std::move(h)) {
    }

    void rr_server::accept(const std::size_t max_message_size) {
        acceptor.async_accept(
            [this, max_message_size](std::error_code ec, asio::ip::tcp::socket socket) {
                const auto address = socket.remote_endpoint().address().to_string();
                if (!ec) {
                    spdlog::info("Opening new TCP session with address {}", address);
                    auto new_session = new session(std::move(socket), this->handler, max_message_size);
                    new_session->start();
                }
                else
                    spdlog::error("Could not open session with address {}", address);

                accept(max_message_size);
            });
    }
} // kv_server