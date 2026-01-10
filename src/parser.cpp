#include "parser.h"

#include <algorithm>
#include <sstream>

namespace min_kv {
    tokenizer::tokenizer(const std::string_view &src): src(src), pos(0) {

        while (!eof()) {
            skip_whitespace();
            if (eof())
                break;
            if (is_identifier_start())
                tokenize_identifier();
            else if (is_string_literal_start())
                tokenize_string();
            else {
                std::stringstream es;
                es << "unexpected character: '" << peek() << "'";
                throw parser_error(es.str().c_str());
            }
        }
    }

    std::vector<token> tokenizer::get_tokens() { return tokens; }
    bool tokenizer::eof() const { return pos >= src.length(); }
    void tokenizer::advance() { pos++; }
    char tokenizer::peek() const {
        if (!eof())
            return src[pos];
        return '\0';
    }

    void tokenizer::skip_whitespace() {
        while (!eof() && std::isspace(peek()))
            advance();
    }

    bool tokenizer::is_identifier_start() const {
        return std::isalpha(peek()) || peek() == '_';
    }

    bool tokenizer::is_string_literal_start() const {
        return peek() == '\"';
    }

    bool tokenizer::is_command_identifier(std::string_view token_string) {
        static const std::string commands[] = {"GET", "SET", "CLOSE", "DELETE", "APPEND", "LOGIN"};
        return std::ranges::any_of(commands, [&token_string](const auto& token)-> bool {
           return token_string == token;
        });
    }

    void tokenizer::tokenize_identifier() {
        token t;
        while ( (std::isalnum(peek()) || peek() == '_') && !eof()) {
            t.value.push_back(peek());
            advance();
        }

        if (is_command_identifier(t.value)) {
            t.type = token_type::command_identifier;
        } else
            t.type = token_type::key_identifier;

        tokens.push_back(t);

    }

    void tokenizer::tokenize_string() {
        token t;
        t.type = token_type::string_literal;
        if (!is_string_literal_start())
            throw parser_error("invalid string literal");
        advance();
        while (!eof() && peek() != '"') {

            // Check for escaped characters
            if (peek() == '\\') {
                advance();
                switch (peek()) {
                    case 'n':
                        t.value.push_back('\n');
                        break;
                    case 't':
                        t.value.push_back('\t');
                        break;
                    case 'r':
                        t.value.push_back('\r');
                        break;
                    case '\\':
                        t.value.push_back('\\');
                        break;
                    case '\"':
                        t.value.push_back('\"');
                        break;
                    case '\'':
                        t.value.push_back('\'');
                        break;
                    default:
                        throw parser_error("invalid escape sequence");
                }
                advance();
            } else {
                // Otherwise collect char for token
                t.value.push_back(peek());
                advance();
            }
        }
        if (eof())
            throw parser_error("String literal missing closing \". ");
        advance();

        tokens.push_back(t);
    }

    command parse_command(const std::vector<token> &tokens) {
        command cmd;

        if (tokens.empty())
            throw parser_error("empty command");
        if (tokens[0].type != token_type::command_identifier)
            throw parser_error("invalid command identifier");
        if (tokens[0].value == "SET") {
            if (tokens.size() < 3)
                throw parser_error("The SET command requires 2 arguments");
            if (tokens[1].type != token_type::key_identifier)
                throw parser_error("Expected a key identifier as the first argument for SET");
            if (tokens[2].type != token_type::string_literal)
                throw parser_error("Expected a string literal as the second argument for SET");
            cmd.type = command_type::SET;
            cmd.args.push_back(tokens[1].value);
            cmd.args.push_back(tokens[2].value);
        } else if (tokens[0].value == "GET") {
            if (tokens.size() < 2)
                throw parser_error("The GET command requires 1 argument");
            if (tokens[1].type != token_type::key_identifier)
                throw parser_error("Expected a key identifier as the argument for GET");
            cmd.type = command_type::GET;
            cmd.args.push_back(tokens[1].value);
        } else if (tokens[0].value == "CLOSE") {
            cmd.type = command_type::CLOSE;
        } else if (tokens[0].value == "DELETE") {
            if (tokens.size() < 2)
                throw parser_error("The DELETE command requires 1 argument");
            if (tokens[1].type != token_type::key_identifier)
                throw parser_error("Expected a key identifier as the argument for DELETE");
            cmd.type = command_type::DELETE;
            cmd.args.push_back(tokens[1].value);
        } else if (tokens[0].value == "APPEND") {
            if (tokens.size() < 3)
                throw parser_error("The APPEND command requires 2 arguments");
            if (tokens[1].type != token_type::key_identifier)
                throw parser_error("Expected a key identifier as the first argument for APPEND");
            if (tokens[2].type != token_type::string_literal)
                throw parser_error("Expected a string literal as the second argument for APPEND");
            cmd.type = command_type::APPEND;
            cmd.args.push_back(tokens[1].value);
            cmd.args.push_back(tokens[2].value);
        } else if (tokens[0].value == "LOGIN") {
            if (tokens.size() < 3)
                throw parser_error("The LOGIN command requires 2 arguments");
            if (tokens[1].type != token_type::key_identifier)
                throw parser_error("Expected a key identifier as the first argument for LOGIN");
            if (tokens[2].type != token_type::string_literal)
                throw parser_error("Expected a string literal as the second argument for LOGIN");
            cmd.type = command_type::LOGIN;
            cmd.args.push_back(tokens[1].value);
            cmd.args.push_back(tokens[2].value);
        }
        else {
            throw parser_error("invalid command");
        }

        return cmd;
    };

};