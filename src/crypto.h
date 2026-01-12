
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

    void combine_hash_salt(hash_t &input_hash, salt_t &input_salt, combined_hash_t &output_combined_hash);
    void split_combined_hash(combined_hash_t &input_combined_hash, hash_t &output_hash, salt_t &output_salt) ;
    bool hash_scrypt(std::string &input_pass, salt_t &output_salt, hash_t &output_hash);
    bool verify_password_scrypt(std::string &input_pass, salt_t &input_salt, hash_t &expected_hash);

    void encode_base64(combined_hash_t &input_combined, std::string &output_encoded);
    void decode_base64(std::string &input_encoded, combined_hash_t &output_decoded);
} // min_kv

#endif //MINKV_CRYPTO_H