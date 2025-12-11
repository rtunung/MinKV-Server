#include "request_response_server.h"

#include <iostream>
#include <asio/write.hpp>
#include <asio.hpp>
#include <utility>

namespace min_kv {

    session::session(asio::ip::tcp::socket socket, handler h) :
        socket_(std::move(socket)), handler_(std::move(h)) {    }

    void session::start() {
        read();
    }

    void session::close() {
        closing_ = true;
    }

    void session::read() {
        auto self = shared_from_this();
        asio::async_read_until(socket_,asio::dynamic_buffer(read_buffer_) ,"\n",
            [this, self](std::error_code ec, std::size_t size) {
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
            });

    }

    void session::write(const std::string &str) {
        auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(str, str.size()),
            [this, self] (std::error_code ec, std::size_t size) {
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
            });
    }

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