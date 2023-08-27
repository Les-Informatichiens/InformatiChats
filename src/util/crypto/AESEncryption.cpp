//
// Created by fofi1 on 2023-08-27.
//

#include "AESEncryption.h"
#include "util/Base64.h"


/**
 * Encrypts the given message with AES using the given key.
 * @param plainText the message to encrypt
 * @param key the key used to encrypt the message
 * @return the encrypted message
 */
std::string EncryptAES(const std::string& plainText, const std::string& key)
{
    // Initialize the library
    OpenSSL_add_all_algorithms();

    // Create and initialize the context
    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Initialize encryption operation
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, (const unsigned char*) key.c_str(), (const unsigned char*) "0123456789012345");

    // Provide the plaintext and get the ciphertext
    std::string encryptedText(plainText.length() + EVP_CIPHER_block_size(EVP_aes_128_cbc()), '\0');
    int len;
    EVP_EncryptUpdate(ctx, (unsigned char*) encryptedText.data(), &len, (const unsigned char*) plainText.c_str(), plainText.length());

    // Finalize the encryption
    int finalLen;
    EVP_EncryptFinal_ex(ctx, (unsigned char*) encryptedText.data() + len, &finalLen);
    len += finalLen;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    EVP_cleanup();

    encryptedText.resize(len);
    return macaron::Base64::Encode(encryptedText);
}

/**
 * Decrypts the given message which was previously encoded with AES using the given key.
 * @param encryptedText the encrypted message
 * @param key the key used to encrypt the message
 * @return the decrypted message
 */
std::string DecryptAES(const std::string& encryptedText, const std::string& key)
{
    // Initialize the library
    OpenSSL_add_all_algorithms();

    // Create and initialize the context
    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Initialize decryption operation
    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, (const unsigned char*) key.c_str(), (const unsigned char*) "0123456789012345");

    // Provide the ciphertext and get the plaintext
    std::string decodedEncryptedText;
    macaron::Base64::Decode(encryptedText, decodedEncryptedText);
    std::string decryptedText(encryptedText.length(), '\0');
    int len;
    EVP_DecryptUpdate(ctx, (unsigned char*) decryptedText.data(), &len, (const unsigned char*) decodedEncryptedText.c_str(), decodedEncryptedText.length());

    // Finalize the decryption
    int finalLen;
    EVP_DecryptFinal_ex(ctx, (unsigned char*) decryptedText.data() + len, &finalLen);
    len += finalLen;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    EVP_cleanup();

    decryptedText.resize(len);
    return decryptedText;
}
