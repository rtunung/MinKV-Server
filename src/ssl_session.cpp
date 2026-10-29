
#include "ssl_session.h"

#include <iostream>
#include <asio.hpp>
#include <utility>


namespace min_kv {
    ssl_session::ssl_session(asio::ip::tcp::socket socket, asio::ssl::context &ssl_ctx, handler_function h) :
        session(std::move(socket), std::move(h))
    {
        this->ssl_stream = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket> >(std::move(session_socket), ssl_ctx);
    }

    void ssl_session::start(){
        build_read_write();
        perform_handshake();
    }

    void ssl_session::read() {
        asio::async_read_until(*ssl_stream,asio::dynamic_buffer(read_buffer) ,"\r\n",read_function);
    }

    void ssl_session::write(const std::string &str) {
        asio::async_write(*ssl_stream, asio::buffer(str, str.size()), write_function);
    }

    void ssl_session::internal_close() {
        ssl_stream->shutdown();
    }

    void ssl_session::perform_handshake() {
        auto self = shared_from_this();
        ssl_stream->async_handshake(asio::ssl::stream_base::server,
            [this, self](asio::error_code ec) {
                if (!ec)
                    read();
                else
                    std::cerr << "ERROR: Handshake failed: " << ec.message() << std::endl;
            });
    }
} // min_kv