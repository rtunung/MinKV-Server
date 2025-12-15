
#include "session.h"

#include <iostream>
#include <asio/write.hpp>
#include <asio.hpp>
#include <utility>

namespace min_kv {
    session::session(asio::ip::tcp::socket socket, handler h) : socket_(std::move(socket)), handler_(std::move(h)){
    }

    void session::start() {
        build_read_write();
        read();
    }

    void session::close() {
        this->closing_ = true;
    }

    void session::read() {
        asio::async_read_until(socket_,asio::dynamic_buffer(read_buffer_) ,"\n",read_function_);

    }

    void session::write(const std::string &str) {
        asio::async_write(socket_, asio::buffer(str, str.size()), write_function_);
    }

    void session::build_read_write() {
        std::cout << "Before shared_from_this" << std::endl;
        auto self = shared_from_this();

        std::cout << "Building reader" << std::endl;

        this->read_function_ = [this, self](std::error_code ec, std::size_t size) {
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

        std::cout << "Building writer" << std::endl;

        this->write_function_ = [this, self] (std::error_code ec, std::size_t size) {
            if (closing_) {
                internal_close();
                std::cout << "INFO: A connection has been closed." << std::endl;
                return;
            }
            if (!ec) {
                read();
            } else {
                std::cerr << "ERROR: An error has occurred while writing data to the socket: " << ec.message() << std::endl;
            }
        };
    }

    void session::internal_close() {
        this->socket_.close();
    }
} // min_kv