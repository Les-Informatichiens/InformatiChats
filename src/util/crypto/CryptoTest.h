//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <util/crypto/AESEncryption.h>
#include <util/crypto/Keygen.h>
#include <util/crypto/RSAEncryption.h>


void TestCryptography()
{
    // RSA keypair generation
    RSAKeyPair keypair = GenerateRSAKeyPair(512);

    const char* privKeyHeader = "-----BEGIN PRIVATE KEY-----";
    const char* privKeyFooter = "-----END PRIVATE KEY-----";
    const char* pubKeyHeader = "-----BEGIN PUBLIC KEY-----";
    const char* pubKeyFooter = "-----END PUBLIC KEY-----";
    // Locate the positions of the header and footer
    size_t privHeaderPos = keypair.privateKey.find(privKeyHeader);
    size_t privFooterPos = keypair.privateKey.find(privKeyFooter);

    std::string extractedPrivKey = keypair.privateKey.substr(privHeaderPos + strlen(privKeyHeader),
                                                             privFooterPos - privHeaderPos - strlen(privKeyHeader));
    extractedPrivKey.erase(std::remove_if(extractedPrivKey.begin(), extractedPrivKey.end(), ::isspace), extractedPrivKey.end());

    // Locate the positions of the header and footer
    size_t pubHeaderPos = keypair.publicKey.find(pubKeyHeader);
    size_t pubFooterPos = keypair.publicKey.find(pubKeyFooter);

    std::string extractedPubKey = keypair.publicKey.substr(pubHeaderPos + strlen(pubKeyHeader),
                                                           pubFooterPos - pubHeaderPos - strlen(pubKeyHeader));
    extractedPubKey.erase(std::remove_if(extractedPubKey.begin(), extractedPubKey.end(), ::isspace), extractedPubKey.end());

    std::cout << "Private key:\n"
              << extractedPrivKey << "\n";
    std::cout << "\nPublic key:\n"
              << extractedPubKey << "\n";

    // Cryptographic key generation from password and username
    std::string derivedKey = DeriveKeyFromPassword("password", "username", 256 / 8);
    std::cout << "Derived key from password:\n"
              << derivedKey << "\n";

    // RSA Private key encryption/decryption
    std::string encryptedPrivKeyAES = EncryptAES("(RSA private key)", derivedKey);
    std::cout << "Encrypted private RSA key with derived key: " << encryptedPrivKeyAES << "\n";

    std::string decryptedPrivKeyAES = DecryptAES(encryptedPrivKeyAES, derivedKey);
    std::cout << "Decrypted private RSA key with derived key: " << decryptedPrivKeyAES << "\n";

    // Hanshake message encryption/decryption
    std::string encryptedPrivKeyRSA = EncryptRSA("top secret handshake message", keypair.publicKey);
    std::cout << "Encrypted message with RSA public key: " << encryptedPrivKeyRSA << "\n";

    std::string decryptedPrivKeyRSA = DecryptRSA(encryptedPrivKeyRSA, keypair.privateKey);
    std::cout << "Decrypted message with RSA private key: " << decryptedPrivKeyRSA << "\n";
}
