#ifndef MINKV_SSL_RR_SERVER_H
#define MINKV_SSL_RR_SERVER_H

#include "rr_server.h"

namespace min_kv {
    class ssl_rr_server : public rr_server {
    public:
        ssl_rr_server(asio::io_context& context, unsigned short port,
            const std::string& cert_file,const std::string& key_file, handler_function h);

    protected:
        void accept() override;

        asio::ssl::context ssl_ctx;
    };
} // min_kv

#endif //MINKV_SSL_RR_SERVER_H