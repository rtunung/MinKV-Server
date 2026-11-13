#ifndef MINKV_CONFIG_H
#define MINKV_CONFIG_H

#include <optional>
#include <unordered_map>
#include <fstream>

namespace min_kv {
    class config {
    public:
        explicit config(const std::string &file_name);

        std::optional<std::string> getStringValue(const std::string &key);
        std::optional<bool> getBoolValue(const std::string &key);
        std::optional<int> getIntValue(const std::string &key);
        std::optional<float> getFloatValue(const std::string &key);

        std::string getFileName();

    private:
        std::unordered_map<std::string, std::string> config_pairs;
        std::string file_name;
    };

    std::string trim_string(const std::string &input);

} // min_kv

#endif //MINKV_CONFIG_H