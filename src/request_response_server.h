#ifndef KV_SERVER_SERVER_H
#define KV_SERVER_SERVER_H

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

    class request_response_server {
    public:
        request_response_server(asio::io_context &context, unsigned short port,
            handler h);

    private:
        void accept();

        asio::ip::tcp::acceptor acceptor_;
        handler handler_;
    };
} // kv_server

#endif //KV_SERVER_SERVER_H