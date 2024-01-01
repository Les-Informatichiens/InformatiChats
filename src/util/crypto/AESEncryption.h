//
// Created by thierrycd on 2023-08-27.
//

#pragma once

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string>


/**
 * Encrypts the given message with AES using the given key.
 * @param plainText the message to encrypt
 * @param key the key used to encrypt the message
 * @return the encrypted message
 */
std::string EncryptAES(const std::string& plainText, const std::string& key);

/**
 * Decrypts the given message which was previously encoded with AES using the given key.
 * @param encryptedText the encrypted message
 * @param key the key used to encrypt the message
 * @return the decrypted message
 */
std::string DecryptAES(const std::string& encryptedText, const std::string& key);
