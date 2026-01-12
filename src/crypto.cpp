
#include "crypto.h"

#include <cstring>
#include <stdexcept>

namespace min_kv {
    void combine_hash_salt(hash_t &input_hash, salt_t &input_salt, combined_hash_t &output_combined_hash) {
        std::memcpy(output_combined_hash.data(), input_hash.data(), HASH_LEN);
        std::memcpy(output_combined_hash.data() + HASH_LEN, input_salt.data(), SALT_LEN);
        OPENSSL_cleanse(input_hash.data(), HASH_LEN);
        OPENSSL_cleanse(input_salt.data(), SALT_LEN);
    }

    void split_combined_hash(combined_hash_t &input_combined_hash, hash_t &output_hash, salt_t &output_salt)  {
        std::memcpy(output_hash.data(), input_combined_hash.data(), HASH_LEN);
        std::memcpy(output_salt.data(), input_combined_hash.data() + HASH_LEN, SALT_LEN);
        OPENSSL_cleanse(input_combined_hash.data(), HASH_LEN);
    }

    bool hash_scrypt(std::string &input_pass, salt_t &output_salt, hash_t &output_hash) {
        int err = RAND_bytes(output_salt.data(), output_salt.size());
        if (err != 1)
            return false;

        int res = EVP_PBE_scrypt(input_pass.data(), input_pass.size(), output_salt.data(), output_salt.size(),
            16384, 8, 1, 64 * 1024 * 1024, output_hash.data(), output_hash.size());

        OPENSSL_cleanse(input_pass.data(), input_pass.size());
        return res == 1;
    }

    bool verify_password_scrypt(std::string &input_pass, salt_t &input_salt, hash_t &expected_hash) {
        hash_t new_hash = {0};

        int res = EVP_PBE_scrypt(input_pass.data(), input_pass.size(), input_salt.data(), input_salt.size(),
            16384, 8, 1, 64 * 1024 * 1024, new_hash.data(), new_hash.size());
        if (res != 1)
            throw std::runtime_error("Password hashing failed");
        bool verified = CRYPTO_memcmp(new_hash.data(), expected_hash.data(), HASH_LEN) == 0;

        OPENSSL_cleanse(new_hash.data(), new_hash.size());
        OPENSSL_cleanse(input_salt.data(), input_salt.size());
        OPENSSL_cleanse(input_pass.data(), input_pass.size());
        OPENSSL_cleanse(expected_hash.data(), expected_hash.size());

        return verified;
    }

    void encode_base64(combined_hash_t &input_combined, std::string &output_encoded) {
        int len = 4 * ((input_combined.size() + 2) / 3);
        output_encoded = std::string(len, '\0');
        EVP_EncodeBlock(reinterpret_cast<unsigned char*>(&output_encoded[0]),input_combined.data(),input_combined.size());
        OPENSSL_cleanse(input_combined.data(), input_combined.size());
    }

    void decode_base64(std::string &input_encoded, combined_hash_t &output_decoded) {
        EVP_DecodeBlock(output_decoded.data(), reinterpret_cast<const unsigned char*>(input_encoded.data()),input_encoded.size());
        OPENSSL_cleanse(input_encoded.data(), input_encoded.size());
    }
} // min_kv