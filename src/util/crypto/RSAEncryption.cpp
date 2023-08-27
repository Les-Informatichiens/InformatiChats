//
// Created by Jonathan Richard on 2023-08-27.
//

#include "RSAEncryption.h"

std::string EncryptRSA(const std::string& plainText, const std::string& publicKey)
{
    // Load public key from provided string
    BIO* bio = BIO_new_mem_buf(publicKey.c_str(), -1);
    if (!bio)
    {
        // Handle error
        return "";
    }

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (!pkey)
    {
        // Handle error
        BIO_free(bio);
        return "";
    }

    // Create/initialize context
    EVP_PKEY_CTX* ctx;
    ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_encrypt_init(ctx);

    // Specify padding: default is PKCS#1 v1.5
    // EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING); // for OAEP with SHA1 for both digests

    // Encryption
    std::string outStr;
    size_t ciphertextLen;
    EVP_PKEY_encrypt(ctx, nullptr, &ciphertextLen, (const unsigned char*) plainText.c_str(), plainText.size());
    unsigned char* ciphertext = (unsigned char*) OPENSSL_malloc(ciphertextLen);
    EVP_PKEY_encrypt(ctx, ciphertext, &ciphertextLen, (const unsigned char*) plainText.c_str(), plainText.size());
    outStr.assign((char*) ciphertext, ciphertextLen);

    // Release memory
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    OPENSSL_free(ciphertext);

    return outStr;
}

std::string DecryptRSA(const std::string& encryptedText, const std::string& privateKey)
{
    // Load public key from provided string
    BIO* bio = BIO_new_mem_buf(privateKey.c_str(), -1);
    if (!bio)
    {
        // Handle error
        return "";
    }

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    if (!pkey)
    {
        // Handle error
        BIO_free(bio);
        return "";
    }

    // Create/initialize context
    EVP_PKEY_CTX* ctx;
    ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_decrypt_init(ctx);

    // Decryption
    std::string outStr;
    size_t deciphertextLen;
    EVP_PKEY_decrypt(ctx, nullptr, &deciphertextLen, (const unsigned char*) encryptedText.c_str(), encryptedText.size());
    unsigned char* deciphertext = (unsigned char*) OPENSSL_malloc(deciphertextLen);
    EVP_PKEY_decrypt(ctx, deciphertext, &deciphertextLen, (const unsigned char*) encryptedText.c_str(), encryptedText.size());
    outStr.assign((char*) deciphertext, deciphertextLen);

    // Release memory
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    OPENSSL_free(deciphertext);

    return outStr;
}
