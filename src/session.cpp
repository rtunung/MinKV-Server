
#include "session.h"

#include <iostream>
#include <asio/write.hpp>
#include <asio.hpp>
#include <utility>

namespace min_kv {
    session::session(asio::ip::tcp::socket socket, handler h) : socket_(std::move(socket)), ssl_stream_(nullptr),
                                                                handler_(std::move(h)), use_ssl_(false) {
    }

    session::session(asio::ip::tcp::socket socket, asio::ssl::context &ssl_ctx, handler h) : socket_(std::move(socket)),
        handler_(std::move(h)), use_ssl_(true) {
        this->ssl_stream_ = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket> >(std::move(socket_), ssl_ctx);
    }

    void session::start() {
        if (use_ssl_)
            perform_handshake();
        else
            read();
    }

    void session::perform_handshake() {
        auto self = shared_from_this();
        ssl_stream_->async_handshake(asio::ssl::stream_base::server,
            [this, self](asio::error_code ec) {
                if (!ec)
                    read();
                else
                    std::cerr << "ERROR: Handshake failed: " << ec.message() << std::endl;
            });
    }

    void session::close() {
        closing_ = true;
    }

    void session::read() {
        auto self = shared_from_this();

        auto f = [this, self](std::error_code ec, std::size_t size) {
            if (!ec) {
                std::string reply = handler_(std::string(read_buffer_), *this);
                write(reply);
                self->read_buffer_.clear();
            } else if (ec == asio::error::eof) {
                std::cout << "INFO: A connection has been closed." << std::endl;
            }
            else {
                std::cerr << "ERROR: An error has occurred while reading on socket: " << ec.message() << std::endl;
            }
        };

        if (use_ssl_)
            asio::async_read_until(*ssl_stream_,asio::dynamic_buffer(read_buffer_) ,"\n",f);
        else
            asio::async_read_until(socket_,asio::dynamic_buffer(read_buffer_) ,"\n",f);

    }

    void session::write(const std::string &str) {
        auto self = shared_from_this();

        auto f = [this, self] (std::error_code ec, std::size_t size) {
            if (closing_) {
                socket_.close();
                std::cout << "INFO: A connection has been closed." << std::endl;
                return;
            }
            if (!ec) {
                read();
            } else {
                std::cerr << "ERROR: An error has occurred while writing data to the socket: " << ec.message() << std::endl;
            }
        };

        if (use_ssl_)
            asio::async_write(*ssl_stream_, asio::buffer(str, str.size()), f);
        else
            asio::async_write(socket_, asio::buffer(str, str.size()), f);
    }

} // min_kv