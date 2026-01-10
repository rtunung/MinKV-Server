
#include "crypto.h"

#include <cstring>
#include <stdexcept>

namespace min_kv {
    void combine_hash_salt(const hash_t &hash, const salt_t &salt, combined_hash_t &combined_hash) {
        std::memcpy(combined_hash.data(), hash.data(), HASH_LEN);
        std::memcpy(combined_hash.data() + HASH_LEN, salt.data(), SALT_LEN);
    }

    void split_combined_hash(const combined_hash_t &combined_hash, hash_t &hash, salt_t &salt)  {
        std::memcpy(hash.data(), combined_hash.data(), HASH_LEN);
        std::memcpy(salt.data(), combined_hash.data() + HASH_LEN, SALT_LEN);
    }

    bool hash_scrypt(const std::string &pass, salt_t &salt_result, hash_t &hash_result) {
        int err = RAND_bytes(salt_result.data(), salt_result.size());
        if (err != 1)
            return false;

        int res = EVP_PBE_scrypt(pass.data(), pass.size(), salt_result.data(), salt_result.size(),
            16384, 8, 1, 64 * 1024 * 1024, hash_result.data(), hash_result.size());
        return res == 1;
    }

    bool verify_password_scrypt(const std::string &pass, const salt_t &salt, const hash_t &expected_hash) {
        hash_t new_hash = {0};

        int res = EVP_PBE_scrypt(pass.data(), pass.size(), salt.data(), salt.size(),
            16384, 8, 1, 64 * 1024 * 1024, new_hash.data(), new_hash.size());
        if (res != 1)
            throw std::runtime_error("Password hashing failed");

        return CRYPTO_memcmp(new_hash.data(), expected_hash.data(), HASH_LEN) == 0;
    }

    std::string encode_base64(const combined_hash_t &combined) {
        int len = 4 * ((combined.size() + 2) / 3);
        std::string encoded_str(len, '\0');
        EVP_EncodeBlock(reinterpret_cast<unsigned char*>(&encoded_str[0]),combined.data(),combined.size());
        return encoded_str;
    }

    void decode_base64(const std::string &encoded, combined_hash_t &decoded) {
        EVP_DecodeBlock(decoded.data(), reinterpret_cast<const unsigned char*>(encoded.data()),encoded.size());

    }
} // min_kv