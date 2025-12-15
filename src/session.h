
#ifndef MINKV_SESSION_H
#define MINKV_SESSION_H

#include <memory>
#include <asio/ip/tcp.hpp>
#include <asio/ssl/stream.hpp>
#include <functional>

namespace min_kv {
    class session;
    typedef std::function<std::string(const std::string&, session &s)> handler;

    class session : public std::enable_shared_from_this<session> {
    public:

        // Non-SSL version
        session(asio::ip::tcp::socket socket, handler h);

        // SSL version
        session(asio::ip::tcp::socket socket, asio::ssl::context& ssl_ctx, handler h);

        void start();
        void close();

    private:
        void read();
        void write(const std::string &str);

        void perform_handshake();

        asio::ip::tcp::socket socket_;

        std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>> ssl_stream_;

        handler handler_;
        std::basic_string<char> read_buffer_;
        bool closing_ = false;
        const bool use_ssl_;
    };
} // min_kv

#endif //MINKV_SESSION_H