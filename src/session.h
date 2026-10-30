
#ifndef MINKV_SESSION_H
#define MINKV_SESSION_H

#include <memory>
#include <asio/ip/tcp.hpp>
#include <asio/ssl/stream.hpp>
#include <functional>

#include "user_db_connection.h"

namespace min_kv {

    // Message limit for now is 10 MB
    constexpr std::size_t MAXIMUM_MESSAGE_SIZE = 1024 * 1024 * 10;

    class session;
    typedef std::function<std::string(const std::string&, session &s)> handler_function;

    class session : public std::enable_shared_from_this<session> {
    public:
        virtual ~session() = default;
        session(asio::ip::tcp::socket socket, handler_function h);

        virtual void start();
        void close();

        bool is_authenticated() const;
        bool read_permission() const;
        bool write_permission() const;

        void set_user(const std::optional<user> &new_user);

    protected:
        virtual void read();
        virtual void write(const std::string &str);

        void build_read_write();
        virtual void internal_close();

        std::string address;

        std::function<void(std::error_code, size_t)> read_function;
        std::function<void(std::error_code, size_t)> write_function;

        asio::ip::tcp::socket session_socket;

        handler_function handler;
        std::basic_string<char> read_buffer;
        bool closing = false;

        std::optional<user> session_user;
    };
} // min_kv

#endif //MINKV_SESSION_H