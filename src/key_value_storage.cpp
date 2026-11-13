
#include "key_value_storage.h"

#include <asio/detail/chrono.hpp>

#include <spdlog/spdlog.h>

namespace min_kv {
    key_value_storage::key_value_storage(const std::uint64_t purge_timer_ms)
        : purge_timer_ms(purge_timer_ms) {}

    std::uint64_t key_value_storage::insert_key_value(const std::string &key, std::string value) {
        kv_map::accessor acc;
        kv_store.insert(acc, key);
        if (!acc.empty()) {
            acc->second.generation = ++generation_counter;
            acc->second.value = std::move(value);
        } else
            acc->second = {.value = std::move(value), .generation = ++generation_counter};

        return acc->second.generation;
    }

    void key_value_storage::insert_key_value_expiration(const std::string &key, std::string value, const int milliseconds) {
        const auto generation = insert_key_value(key, std::move(value));

        expiration_queue_mutex.lock();
        expiration_queue.push({
            .key = key,
            .generation = generation,
            .expires_at = std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds)});
        expiration_queue_mutex.unlock();
    }

    std::string key_value_storage::get_value(const std::string &key) const {
        kv_map::const_accessor acc;
        kv_store.find(acc, key);
        if (acc.empty()) {
            return "";
        }
        return acc->second.value;
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
        acc->second.value.append(append);
    }

    void key_value_storage::append_key_value_expiration(const std::string &key, const std::string &append, const int milliseconds) {
        kv_map::accessor acc;
        kv_store.find(acc, key);
        create_if_empty(acc, key);
        acc->second.value.append(append);
        acc->second.generation = ++generation_counter;

        const auto generation = acc->second.generation;

        // This release is important, we don't want to hold an accessor and the expiration_queue mutex at the same time
        // or we can create a potential deadlock with the thread running purge_expired_entries()
        acc.release();

        expiration_queue_mutex.lock();
        expiration_queue.push({
            .key = key,
            .generation = generation,
            .expires_at = std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds)});
        expiration_queue_mutex.unlock();
    }

    void key_value_storage::purge_expired_entries() {
        this->purge_entries = true;

        while (purge_entries) {
            expiration_queue_mutex.lock();
            while (!expiration_queue.empty() && expiration_queue.top().expires_at < std::chrono::steady_clock::now()) {
                const auto current = expiration_queue.top();
                kv_map::accessor acc;
                kv_store.find(acc, current.key);
                if (!acc.empty() && current.generation == acc->second.generation)
                    kv_store.erase(acc);
                expiration_queue.pop();
            }
            expiration_queue_mutex.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(purge_timer_ms));
        }
    }

    void key_value_storage::stop_purge_expired_entries() {
        this->purge_entries = false;
    }

    void key_value_storage::create_if_empty(kv_map::accessor &acc, const std::string& key) {
        if (acc.empty()) {
            kv_store.insert(acc, key);
            acc->second = {.value = "", .generation = ++generation_counter};
        }
    }
} // min_kv