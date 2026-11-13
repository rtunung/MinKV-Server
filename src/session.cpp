
#include "session.h"

#include <iostream>
#include <asio/write.hpp>
#include <asio.hpp>
#include <utility>
#include <spdlog/spdlog.h>

namespace min_kv {
    session::~session() {
        spdlog::debug("session for {} destroyed", address);
    }

    session::session(asio::ip::tcp::socket socket, handler_function h, const std::size_t maximum_message_size) :
    session_socket(std::move(socket)), handler(std::move(h)), maximum_message_size(maximum_message_size) {
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

    void session::set_user(std::optional<user> new_user) {
        this->session_user = std::move(new_user);
    }

    void session::read() {
        asio::async_read_until(session_socket,asio::dynamic_buffer(read_buffer, maximum_message_size) ,"\r\n",read_function);
    }

    void session::write(const std::string &str) {
        asio::async_write(session_socket, asio::buffer(str, str.size()), write_function);
    }

    void session::build_read_write() {

        this->read_function = [this](const std::error_code ec, std::size_t size) {
            if (!ec) {
                const std::string reply = this->handler(std::string(this->read_buffer), *this);
                this->write(reply);
                this->read_buffer.clear();
                return;
            } if (ec == asio::error::eof) {
                spdlog::info("Connection to client {} has been closed", this->address);
            } else if (ec == asio::error::not_found) {
                spdlog::warn("Message from {} exceeded memory limit. Shutting down connection", this->address);
            } else {
                spdlog::error("Could not read from socket connection to client {}. {}", this->address, ec.message());
            }
            delete this;
        };

        this->write_function = [this] (const std::error_code ec, std::size_t size) {
            if (this->closing) {
                this->internal_close();
                spdlog::info("Closing connection to client {}", this->address);
                delete this;
                return;
            }

            if (!ec) {
                this->read();
            } else {
                spdlog::error("Could not write to socket connecting to client {}. {}", this->address, ec.message());
                delete this;
            }
        };
    }

    void session::internal_close() {
        this->session_socket.close();
    }
} // min_kv