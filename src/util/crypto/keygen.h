//
// Created by tcdemers on 2023-08-26.
//

#pragma once

#include <cstring>
#include <iostream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>


/**
 * Represents an RSA key pair.
 */
struct RSAKeyPair
{
    std::string privateKey;
    std::string publicKey;
};


/**
 * Generates a new RSA key pair with the given key length.
 * @param keyLength the length of the key to generate
 */
[[nodiscard]] RSAKeyPair GenerateRSAKeyPair(size_t keyLength);
