#include "min_kv_server.h"

#include <iostream>

#include "ssl_rr_server.h"
#include "parser.h"

namespace min_kv {
    min_kv_server::min_kv_server(unsigned int port) : io(asio::io_context()) {
        num_threads = 1;
        server = std::make_unique<rr_server>(io, port, build_handler());
    }

    min_kv_server::min_kv_server(unsigned int port, const std::string &cert_file, const std::string &key_file) {
        num_threads = 1;
        server = std::make_unique<ssl_rr_server>(io, port, cert_file, key_file, build_handler());
    }

    min_kv_server::min_kv_server(config &conf) {
        auto port_result = conf.getIntValue("port");
        if (!port_result)
            throw std::runtime_error("port not set");
        int port = port_result.value();

        auto tls = conf.getBoolValue("tls");
        if (!tls)
            throw std::runtime_error("tls not set");
        bool tls_mode = tls.value();

        auto num_threads_result = conf.getIntValue("num_threads");
        if (!num_threads_result)
            throw std::runtime_error("num_threads not set");
        this->num_threads = num_threads_result.value();

        if (tls_mode) {
            std::string key_file = conf.getStringValue("key_file");
            std::string cert_file = conf.getStringValue("cert_file");
            server = std::make_unique<ssl_rr_server>(io, port, cert_file, key_file, build_handler());
        } else {
            server = std::make_unique<rr_server>(io, port, build_handler());
        }

    }

    void min_kv_server::run() {
        server->accept();
        asio::thread_pool thread_pool(num_threads);
        for (std::size_t i = 0; i < num_threads; i++) {
            asio::post(thread_pool, [this]() {
                this->io.run();
            });
        }
        thread_pool.join();
    }

    handler_function min_kv_server::build_handler() {
        return [this] (const std::string &msg, session &s) -> std::string {
            command cmd = {command_type::FAILED_PARSE};
            std::stringstream reply_stream;
            try {
                tokenizer tokens(msg);
                cmd = parse_command(tokens.get_tokens());
            } catch (const parser_error &p_error) {
                reply_stream << "ERROR \"" << p_error.what() << "\"";
            } catch (const std::exception &error) {
                std::cerr << "Error during parsing: " << error.what() << std::endl;
                reply_stream << "ERROR \"Message could not be parsed.\"";
            }

            switch (cmd.type) {
                case command_type::SET:
                    kv_storage.insert_key_value(cmd.args[0], cmd.args[1]);
                    reply_stream << "OK";
                    break;
                    case command_type::GET:
                    reply_stream << "RESULT \"" << kv_storage.get_value(cmd.args[0]) << "\"";
                    break;
                    case command_type::CLOSE:
                    s.close();
                    reply_stream << "OK";
                    break;
                    case command_type::DELETE:
                    kv_storage.delete_key_value(cmd.args[0]);
                    reply_stream << "OK";
                    break;
                    case command_type::APPEND:
                    kv_storage.append_key_value(cmd.args[0], cmd.args[1]);
                    reply_stream << "OK";
                    break;
            }

            reply_stream << "\r\n";
            return reply_stream.str();
        };
    }
} // kv_server