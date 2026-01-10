
#ifndef MINKV_CRYPTO_H
#define MINKV_CRYPTO_H

#include <array>
#include <string>

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace min_kv {
    constexpr size_t SALT_LEN = 16;
    constexpr size_t HASH_LEN = 32;

    using salt_t = std::array<uint8_t, SALT_LEN>;
    using hash_t = std::array<uint8_t, HASH_LEN>;
    constexpr size_t COMBINED_HASH_LEN = HASH_LEN + SALT_LEN;
    using combined_hash_t = std::array<uint8_t, COMBINED_HASH_LEN>;

    void combine_hash_salt(const hash_t& hash, const salt_t& salt, combined_hash_t& combined_hash);
    void split_combined_hash(const combined_hash_t& combined_hash, hash_t& hash, salt_t& salt);
    bool hash_scrypt(const std::string& pass, salt_t& salt_result, hash_t& hash_result);
    bool verify_password_scrypt(const std::string& pass, const salt_t& salt, const hash_t& expected_hash);

    std::string encode_base64(const combined_hash_t& combined);
    void decode_base64(const std::string& encoded, combined_hash_t& decoded);
} // min_kv

#endif //MINKV_CRYPTO_H