
#ifndef MINKV_SSL_SESSION_H
#define MINKV_SSL_SESSION_H

#include "session.h"

namespace min_kv {
    class ssl_session : public session {
    public:
        ssl_session(asio::ip::tcp::socket socket, asio::ssl::context& ssl_ctx, handler h);

        void start() override;

    protected:
        void read() override;
        void write(const std::string &str) override;
        void internal_close() override;

        void perform_handshake();
        std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>> ssl_stream_;
    };
} // min_kv

#endif //MINKV_SSL_SESSION_H