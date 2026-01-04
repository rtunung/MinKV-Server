#include "config.h"

#include <algorithm>

namespace min_kv {
    config::config(const std::string &file_name) {
        std::ifstream file;
        file.open(file_name);
        if (!file.is_open()) {
            throw std::runtime_error("Can't open config file");
        }

        while (!file.eof()) {
            std::string line;
            std::getline(file, line);

            static constexpr char delimiter = '=';
            std::size_t delimiter_pos = line.find(delimiter);
            if (delimiter_pos == std::string::npos)
                continue;

            std::string key =  trim_string( line.substr(0, delimiter_pos));
            std::string value = trim_string( line.substr(delimiter_pos + 1, line.length()));

            config_pairs.insert_or_assign(std::string(key), std::string(value));
        }

        file.close();
    }

    std::string config::getStringValue(const std::string &key){
        return config_pairs[key];
    }

    std::optional<bool> config::getBoolValue(const std::string &key) {
        auto value = config_pairs[key];
        std::ranges::transform(value, value.begin(), ::tolower);
        if (value == "true") {
            return true;
        } else if (value == "false") {
            return false;
        } else {
            return {};
        }
    }

    std::optional<int> config::getIntValue(const std::string &key) {
        const auto value = config_pairs[key];
        try {
            return std::stoi(value);
        } catch (const std::exception &e) {
            return {};
        }
    }

    std::optional<float> config::getFloatValue(const std::string &key){
        const auto value = config_pairs[key];
        try {
            return std::stof(value);
        } catch (const std::exception &e) {
            return {};
        }
    }

    std::string trim_string(const std::string &input) {
        const auto start = std::find_if_not(std::begin(input), std::end(input), isspace);
        const auto end = std::find_if_not(std::rbegin(input), std::rend(input), isspace).base();

        if (start < end) {
            return std::string(start, end);
        }
        return "";
    }
} // min_kv