//
// Created by fofi1 on 2023-08-27.
//

#include "RSAKeygen.h"


/**
 * Generates a new RSA key pair with the given key length.
 * @param keyLength the length of the key to generate
 */
RSAKeyPair GenerateRSAKeyPair(size_t keyLength)
{
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Create a new RSA key context
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);

    // Initialize the context for key generation
    if (EVP_PKEY_keygen_init(ctx) <= 0)
    {
        std::cerr << "Error initializing key generation context." << std::endl;
        return {};
    }

    // Set RSA key length
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, keyLength) <= 0)
    {
        std::cerr << "Error setting RSA key length." << std::endl;
        return {};
    }

    // Generate key pair
    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
    {
        std::cerr << "Error generating RSA key pair." << std::endl;
        return {};
    }

    // Clean up the context
    EVP_PKEY_CTX_free(ctx);

    // Print private key
    BIO* private_key_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(private_key_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
    char* private_key_str;
    long private_key_len = BIO_get_mem_data(private_key_bio, &private_key_str);

    // Print public key
    BIO* public_key_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(public_key_bio, pkey);
    char* public_key_str;
    long public_key_len = BIO_get_mem_data(public_key_bio, &public_key_str);

    // Convert to std::string
    std::string private_key = std::string(private_key_str, private_key_len);
    std::string public_key = std::string(public_key_str, public_key_len);

    // Clean up
    EVP_PKEY_free(pkey);
    BIO_free_all(private_key_bio);
    BIO_free_all(public_key_bio);

    ERR_free_strings();
    EVP_cleanup();

    return {private_key, public_key};
}
