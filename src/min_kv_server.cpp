#include "min_kv_server.h"

#include <iostream>

#include "ssl_rr_server.h"

namespace min_kv {
    min_kv_server::min_kv_server(unsigned int port) : io_(asio::io_context()) {
        server_ = std::make_unique<rr_server>(io_, port, build_handler());
    }

    min_kv_server::min_kv_server(unsigned int port, const std::string &cert_file, const std::string &key_file) {
        server_ = std::make_unique<ssl_rr_server>(io_, port, cert_file, key_file, build_handler());
    }

    void min_kv_server::run() {
        io_.run();
    }

    void min_kv_server::insert_key_value(const std::string& ky, std::string value) {
        key_value_storage_.insert_or_assign(ky, value);
    }

    std::string min_kv_server::get_value(const std::string& ky) {
        return key_value_storage_[ky];
    }

    void min_kv_server::delete_key_value(const std::string &ky) {
        key_value_storage_.erase(ky);
    }

    handler min_kv_server::build_handler() {
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
                    this->insert_key_value(cmd.args[0], cmd.args[1]);
                    reply_stream << "OK";
                    break;
                    case command_type::GET:
                    reply_stream << "RESULT \"" << this->get_value(cmd.args[0]) << "\"";
                    break;
                    case command_type::CLOSE:
                    s.close();
                    reply_stream << "OK";
                    break;
                    case command_type::DELETE:
                    this->delete_key_value(cmd.args[0]);
                    reply_stream << "OK";
                    break;
                    case command_type::APPEND:
                    const auto new_str = this->get_value(cmd.args[0]).append(cmd.args[1]);
                    this->insert_key_value(cmd.args[0], new_str);
                    reply_stream << "OK";
                    break;
            }

            reply_stream << "\r\n";
            return reply_stream.str();
        };
    }
} // kv_server