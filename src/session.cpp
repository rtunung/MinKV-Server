
#include "session.h"

#include <iostream>
#include <asio/write.hpp>
#include <asio.hpp>
#include <utility>

namespace min_kv {
    session::session(asio::ip::tcp::socket socket, handler_function h) : session_socket(std::move(socket)), handler(std::move(h)){
    }

    void session::start() {
        build_read_write();
        read();
    }

    void session::close() {
        this->closing = true;
    }

    bool session::is_authenticated() const {
        return session_user != nullptr;
    }

    bool session::read_permission() const {
        if (session_user != nullptr)
            return session_user->read_permission;
        return false;
    }

    bool session::write_permission() const {
        if (session_user != nullptr)
            return session_user->write_permission;
        return false;
    }

    void session::set_user(std::unique_ptr<user> new_user) {
        this->session_user = std::move(new_user);
    }

    void session::read() {
        asio::async_read_until(session_socket,asio::dynamic_buffer(read_buffer) ,"\r\n",read_function);
    }

    void session::write(const std::string &str) {
        asio::async_write(session_socket, asio::buffer(str, str.size()), write_function);
    }

    void session::build_read_write() {
        auto self = shared_from_this();

        this->read_function = [this, self](std::error_code ec, std::size_t size) {
            if (!ec) {
                std::string reply = handler(std::string(read_buffer), *this);
                write(reply);
                self->read_buffer.clear();
            } else if (ec == asio::error::eof) {
                std::cout << "INFO: A connection has been closed." << std::endl;
            }
            else {
                std::cerr << "ERROR: An error has occurred while reading on socket: " << ec.message() << std::endl;
            }
        };

        this->write_function = [this, self] (std::error_code ec, std::size_t size) {
            if (closing) {
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
        this->session_socket.close();
    }
} // min_kv