
#ifndef MINKV_KEY_VALUE_STORAGE_H
#define MINKV_KEY_VALUE_STORAGE_H

#include <queue>
#include <tbb/concurrent_hash_map.h>

namespace min_kv {
    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    struct entry {
        std::string value;
        std::uint64_t generation;
    };

    struct expiration {
        std::string key;
        std::uint64_t generation;
        time_point expires_at;
    };

    class expiration_compare {
    public:
        bool operator() (const expiration& lhs, const expiration& rhs) const {
            return lhs.expires_at > rhs.expires_at;
        }
    };

    using kv_map = tbb::concurrent_hash_map<std::string, entry>;

    constexpr std::uint64_t DEFAULT_PURGE_TIMER_MS = 25;

    class key_value_storage {
    public:
        explicit key_value_storage(std::uint64_t purge_timer_ms);

        std::uint64_t insert_key_value(const std::string& key, std::string value);
        void insert_key_value_expiration(const std::string& key, std::string value, int milliseconds);
        std::string get_value(const std::string& key) const;
        void delete_key_value(const std::string& key);
        void append_key_value(const std::string& key, const std::string &append);
        void append_key_value_expiration(const std::string& key, const std::string &append, int milliseconds);

        void purge_expired_entries();
        void stop_purge_expired_entries();

    private:
        void create_if_empty(kv_map::accessor& acc, const std::string& key);

        kv_map kv_store;
        std::priority_queue<expiration, std::vector<expiration>, expiration_compare> expiration_queue;
        std::atomic_uint64_t generation_counter = 0;
        std::mutex expiration_queue_mutex;
        bool purge_entries{};

        std::uint64_t purge_timer_ms;
    };
} // min_kv

#endif //MINKV_KEY_VALUE_STORAGE_H