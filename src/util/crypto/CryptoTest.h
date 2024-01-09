//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include "Hashing.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
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
              << extractedPubKey << "\n\n";

    // Cryptographic key generation from password and username
    std::string derivedKey = DeriveKeyFromPassword("password", "username", 256 / 8);
    std::cout << "Derived key from password and username:\n"
              << derivedKey << "\n\n";

    // RSA Private key encryption/decryption
    std::string someRsaPrivKey = "(RSA private key)";
    std::string encryptedPrivKeyAES = EncryptAES(someRsaPrivKey, derivedKey);
    std::cout << "Encrypted private RSA key with derived key: \noriginal: " << someRsaPrivKey << "\nencrypted: " << encryptedPrivKeyAES << "\n";

    std::string decryptedPrivKeyAES = DecryptAES(encryptedPrivKeyAES, derivedKey);
    std::cout << "decrypted: " << decryptedPrivKeyAES << "\n\n";

    // Hanshake message encryption/decryption
    std::string someHandshakeMessage = "top secret handshake message";
    std::string encryptedPrivKeyRSA = EncryptRSA(someHandshakeMessage, keypair.publicKey);
    std::cout << "Encrypted message with RSA public key: \noriginal: " << someHandshakeMessage << "\nencrypted: " << encryptedPrivKeyRSA << "\n";

    std::string decryptedPrivKeyRSA = DecryptRSA(encryptedPrivKeyRSA, keypair.privateKey);
    std::cout << "decrypted: " << decryptedPrivKeyRSA << "\n\n";
}

void Test2()
{
//    // Peer B generates its own secret key (keep it secret)
//    const char* secretKeyB = "peer_b_secret_key";
//    const char* secretKeyA = "jdaosidjajsoaisdaoiasjdsodjoajdoj";
//
//    // Peer A generates a random nonce (one-time token)
//    const char* nonce = "random_nonce_generated_by_peer_a";
//
//    unsigned char* outc = nullptr;
//    // Peer B generates a challenge by hashing the received nonce with its secret key
//    std::string challenge = Hashing::SHA256(secretKeyB, nonce, outc);
//    std::string outcStr = (char*)outc;
//    // Simulate sending the challenge to Peer A (in a real scenario, it would be sent over the network)
//
//    unsigned char* respA = nullptr;
//    // Peer A computes its response (same process as challenge generation)
//    std::string responseA = Hashing::SHA256(secretKeyA, challenge.c_str(), respA);
//    std::string respAStr = (char*)respA;
//
//
//    // Peer B receives the response from Peer A and verifies it
//    bool isValidResponse = (memcmp(outc, respA, SHA256_DIGEST_LENGTH) == 0);//responseA == challenge;
//
//    if (isValidResponse)
//    {
//        std::cout << "Communication setup allowed." << std::endl;
//    }
//    else
//    {
//        std::cout << "Communication setup denied." << std::endl;
//    }
    // Peer B generates its own secret key (keep it secret)
    const char* secretKeyB = "peer_b_secret_key";
    const char* secretKeyA = "jdaosidjajsoaisdaoiasjdsodjoajdoj";

    // Peer A generates a random nonce (one-time token)
    const char* nonce = "random_nonce_generated_by_peer_a";

    // Peer B generates a challenge by hashing the received nonce with its secret key
    unsigned char* challenge = nullptr;
    unsigned int len = -1;
    challenge = HMAC(EVP_sha256(),(unsigned char*) secretKeyB, strlen(secretKeyB), (unsigned char*) nonce, strlen(nonce), challenge, &len);

    // Simulate sending the challenge to Peer A (in a real scenario, it would be sent over the network)

    // Peer A computes its response (same process as challenge generation)
    unsigned char* responseA = nullptr;
    unsigned int len2 = -1;
    responseA = HMAC(EVP_sha256(),(unsigned char*) secretKeyA, strlen(secretKeyA), (unsigned char*) challenge, len, responseA, &len2);

    // Peer B receives the response from Peer A and verifies it
    bool isValidResponse = (memcmp(challenge, responseA, SHA256_DIGEST_LENGTH) == 0);

    if (isValidResponse)
    {
        std::cout << "Communication setup allowed." << std::endl;
    }
    else
    {
        std::cout << "Communication setup denied." << std::endl;
    }
};
