
#ifndef MINKV_SESSION_H
#define MINKV_SESSION_H

#include <memory>
#include <asio/ip/tcp.hpp>
#include <asio/ssl/stream.hpp>
#include <functional>

namespace min_kv {
    class session;
    typedef std::function<std::string(const std::string&, session &s)> handler_function;

    class session : public std::enable_shared_from_this<session> {
    public:
        virtual ~session() = default;
        session(asio::ip::tcp::socket socket, handler_function h);

        virtual void start();
        void close();

    protected:
        virtual void read();
        virtual void write(const std::string &str);

        void build_read_write();
        virtual void internal_close();

        std::function<void(std::error_code, size_t)> read_function;
        std::function<void(std::error_code, size_t)> write_function;

        asio::ip::tcp::socket session_socket;

        handler_function handler;
        std::basic_string<char> read_buffer;
        bool closing = false;
    };
} // min_kv

#endif //MINKV_SESSION_H