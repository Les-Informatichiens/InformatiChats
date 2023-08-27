//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <openssl/pem.h>
#include <string>

std::string EncryptRSA(const std::string& plainText, const std::string& publicKey);
std::string DecryptRSA(const std::string& encryptedText, const std::string& privateKey);
