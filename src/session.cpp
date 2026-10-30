
#include "session.h"

#include <iostream>
#include <asio/write.hpp>
#include <asio.hpp>
#include <utility>
#include <spdlog/spdlog.h>

namespace min_kv {
    session::session(asio::ip::tcp::socket socket, handler_function h) :
    session_socket(std::move(socket)), handler(std::move(h)){
        this->address = session_socket.remote_endpoint().address().to_string();
    }

    void session::start() {
        build_read_write();
        read();
    }

    void session::close() {
        this->closing = true;
    }

    bool session::is_authenticated() const {
        return session_user.has_value();
    }

    bool session::read_permission() const {
        if (session_user.has_value())
            return session_user->read_permission;
        return false;
    }

    bool session::write_permission() const {
        if (session_user.has_value())
            return session_user->write_permission;
        return false;
    }

    void session::set_user(const std::optional<user> &new_user) {
        this->session_user = new_user;
    }

    void session::read() {
        asio::async_read_until(session_socket,asio::dynamic_buffer(read_buffer, MAXIMUM_MESSAGE_SIZE) ,"\r\n",read_function);
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
                spdlog::info("Connection to client {} has been closed", address);
            } else if (ec == asio::error::not_found) {
                spdlog::warn("Message from {} exceeded memory limit. Shutting down connection", address);
            }
            else {
                spdlog::error("Could not read from socket connection to client {}. {}", address, ec.message());
            }
        };

        this->write_function = [this, self] (std::error_code ec, std::size_t size) {
            if (closing) {
                internal_close();
                spdlog::info("Closing connection to client {}", address);
                return;
            }
            if (!ec) {
                read();
            } else {
                spdlog::error("Could not write to socket connecting to client {}. {}", address, ec.message());
            }
        };
    }

    void session::internal_close() {
        this->session_socket.close();
    }
} // min_kv