
#include "key_value_storage.h"

namespace min_kv {
    key_value_storage::key_value_storage() = default;

    void key_value_storage::insert_key_value(const std::string &key, std::string value) {
        kv_map::accessor acc;
        kv_store.insert(acc, key);
        acc->second = std::move(value);
    }

    std::string key_value_storage::get_value(const std::string &key) const {
        kv_map::const_accessor acc;
        kv_store.find(acc, key);
        if (acc.empty()) {
            return "";
        }
        return acc->second;
    }

    void key_value_storage::delete_key_value(const std::string &key) {
        kv_map::accessor acc;
        kv_store.find(acc, key);
        kv_store.erase(acc);
    }

    void key_value_storage::append_key_value(const std::string &key, const std::string &append) {
        kv_map::accessor acc;
        kv_store.find(acc, key);
        create_if_empty(acc, key);
        acc->second.append(append);
    }

    void key_value_storage::create_if_empty(kv_map::accessor &acc, const std::string& key) {
        if (acc.empty()) {
            kv_store.insert(acc, key);
            acc->second = "";
        }
    }
} // min_kv