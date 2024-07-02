//
// Created by Jonathan Richard on 2023-09-30.
//

#include "Hashing.h"
#include "util/Base64.h"

#include <iomanip>
#include <ios>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>
#include <sstream>

std::string Hashing::SHA256(const char* key, const char* data, unsigned char*& out)
{
    std::string outStr;
    unsigned char* hash = nullptr;
    unsigned int len;
    out = HMAC(EVP_sha256(), (unsigned char*) key, strlen(key), (unsigned char*) data, strlen(data), out, &len);

    //    if (hash != nullptr)
    {
        outStr.assign(strdup((char*) out), len);
        // Clean up the allocated memory for the hash
        //        OPENSSL_free(hash);
    }
    return outStr;
}

std::string Hashing::GeneratePublicKeyFingerprint(const std::string& key)
{
    std::string fingerprint;

    // Load the key from the string representation
    BIO* key_bio = BIO_new_mem_buf(key.c_str(), -1); // -1 indicates null-terminated string
    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(key_bio, nullptr, nullptr, nullptr);

    if (pkey) {
        // Create a context for SHA-256
        EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
        EVP_DigestInit(md_ctx, EVP_sha256());

        // Get the raw bytes of the public key
        size_t key_size = i2d_PUBKEY(pkey, nullptr);
        unsigned char* key_bytes = new unsigned char[key_size];
        unsigned char* ptr = key_bytes;
        i2d_PUBKEY(pkey, &ptr);

        // Update the context with the key bytes
        EVP_DigestUpdate(md_ctx, key_bytes, key_size);

        // Calculate the SHA-256 hash
        unsigned char hash[SHA256_DIGEST_LENGTH];
        EVP_DigestFinal(md_ctx, hash, nullptr);

        // Convert the hash bytes to a hexadecimal string
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << std::setw(2) << static_cast<unsigned>(hash[i]);
        }
        fingerprint = ss.str();

        delete[] key_bytes;
        EVP_PKEY_free(pkey);
        BIO_free(key_bio);
        EVP_MD_CTX_free(md_ctx);
    }

    return fingerprint;
}
