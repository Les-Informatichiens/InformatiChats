//
// Created by Jonathan Richard on 2023-08-27.
//

#include "RSAEncryption.h"
#include "util/Base64.h"


/**
 * Encrypts a plain text using RSA public key.
 *
 * @param plainText The plain text to be encrypted.
 * @param publicKey The RSA public key used for encryption.
 * @return The Base64-encoded encrypted message.
 */
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

    return macaron::Base64::Encode(outStr);
}

/**
 * Decrypts an encrypted message using RSA private key.
 *
 * @param encryptedText The Base64-encoded encrypted message.
 * @param privateKey The RSA private key used for decryption.
 * @return The decrypted plain text.
 */
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
    std::string decodedEncryptedText;
    macaron::Base64::Decode(encryptedText, decodedEncryptedText);

    std::string outStr;
    size_t deciphertextLen;
    EVP_PKEY_decrypt(ctx, nullptr, &deciphertextLen, (const unsigned char*) decodedEncryptedText.c_str(), decodedEncryptedText.size());
    unsigned char* deciphertext = (unsigned char*) OPENSSL_malloc(deciphertextLen);
    EVP_PKEY_decrypt(ctx, deciphertext, &deciphertextLen, (const unsigned char*) decodedEncryptedText.c_str(), decodedEncryptedText.size());
    outStr.assign((char*) deciphertext, deciphertextLen);

    // Release memory
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    OPENSSL_free(deciphertext);

    return outStr;
}

/**
 * Validates the correctness of RSA encryption and decryption keys.
 *
 * @param privateKey The RSA private key to validate.
 * @param publicKey The RSA public key to validate.
 * @return True if encryption and decryption are successful and match, false otherwise.
 */
bool ValidateKeysRSA(const std::string& privateKey, const std::string& publicKey)
{
    std::string testStr = "testStr";
    return testStr == DecryptRSA(EncryptRSA(testStr, publicKey), privateKey);
}
