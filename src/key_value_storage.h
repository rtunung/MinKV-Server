
#ifndef MINKV_KEY_VALUE_STORAGE_H
#define MINKV_KEY_VALUE_STORAGE_H

#include <tbb/concurrent_hash_map.h>

namespace min_kv {
    using kv_map = tbb::concurrent_hash_map<std::string, std::string>;

    class key_value_storage {
    public:
        key_value_storage();

        void insert_key_value(const std::string& key, std::string value);
        std::string get_value(const std::string& key);
        void delete_key_value(const std::string& key);
        void append_key_value(const std::string& key, const std::string &append);

    private:
        void create_if_empty(kv_map::accessor& acc, const std::string& key);

        kv_map kv_store;
    };
} // min_kv

#endif //MINKV_KEY_VALUE_STORAGE_H