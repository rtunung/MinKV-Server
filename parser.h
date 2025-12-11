#ifndef MINKV_PARSER_H
#define MINKV_PARSER_H

#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>

namespace min_kv {

    // ---------------------
    // Token Definitions
    // ---------------------
    enum class token_type {
        key_identifier,
        command_identifier,
        string_literal,
    };

    struct token {
        token_type type;
        std::string value;
    };

    // -----------------------
    // Command Definitions
    // -----------------------
    enum class command_type {
        SET,
        GET,
        CLOSE,
        FAILED_PARSE,
    };

    struct command {
        command_type type;
        std::vector<std::string> args;
    };

    // ----------------------
    // Parser Error
    // ----------------------
    class parser_error : public std::runtime_error {
    public:
        explicit parser_error(const char *str) : std::runtime_error(str) {}
    };

    // ----------------------
    // Tokenizer
    // ----------------------
    class tokenizer {
    public:
        explicit tokenizer(const std::string_view &src);

        std::vector<token> get_tokens();

    private:

        // Helper Methods
        bool eof() const;
        void advance();
        char peek() const;

        void skip_whitespace();
        bool is_identifier_start() const;
        bool is_string_literal_start() const;
        static bool is_command_identifier(std::string_view token_string);

        // Lex Tokens
        void tokenize_identifier();
        void tokenize_string();

        // Member Variables
        std::string_view src;
        std::vector<token> tokens;
        size_t pos;
};

    // ---------------------
    // The Parser Function
    // ---------------------
    command parse_command(const std::vector<token> &tokens);

}

#endif //MINKV_PARSER_H