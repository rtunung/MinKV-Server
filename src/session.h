
#ifndef MINKV_SESSION_H
#define MINKV_SESSION_H

#include <memory>
#include <asio/ip/tcp.hpp>
#include <functional>

namespace min_kv {
    class session;
    typedef std::function<std::string(const std::string&, session &s)> handler;

    class session : public std::enable_shared_from_this<session> {
    public:
        session(asio::ip::tcp::socket socket, handler h);
        void start();
        void close();

    private:
        void read();
        void write(const std::string &str);

        asio::ip::tcp::socket socket_;
        handler handler_;
        std::basic_string<char> read_buffer_;
        bool closing_ = false;
    };
} // min_kv

#endif //MINKV_SESSION_H