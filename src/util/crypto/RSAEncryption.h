//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <openssl/pem.h>
#include <string>

/**
 * Encrypts a plain text using RSA public key.
 *
 * @param plainText The plain text to be encrypted.
 * @param publicKey The RSA public key used for encryption.
 * @return The Base64-encoded encrypted message.
 */
std::string EncryptRSA(const std::string& plainText, const std::string& publicKey);

/**
 * Decrypts an encrypted message using RSA private key.
 *
 * @param encryptedText The Base64-encoded encrypted message.
 * @param privateKey The RSA private key used for decryption.
 * @return The decrypted plain text.
 */
std::string DecryptRSA(const std::string& encryptedText, const std::string& privateKey);

/**
 * Validates the correctness of RSA encryption and decryption keys.
 *
 * @param privateKey The RSA private key to validate.
 * @param publicKey The RSA public key to validate.
 * @return True if encryption and decryption are successful and match, false otherwise.
 */
bool ValidateKeysRSA(const std::string& privateKey, const std::string& publicKey);
