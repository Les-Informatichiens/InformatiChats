//
// Created by Jonathan Richard on 2023-09-30.
//

#include "Hashing.h"
#include "util/Base64.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>

std::string Hashing::SHA256(const char* key, const char* data, unsigned char*& out)
{
    std::string outStr;
    unsigned char* hash = nullptr;
    unsigned int len;
    out = HMAC(EVP_sha256(),(unsigned char*) key, strlen(key), (unsigned char*) data, strlen(data), out, &len);

//    if (hash != nullptr)
    {
        outStr.assign(strdup((char*)out), len);
        // Clean up the allocated memory for the hash
//        OPENSSL_free(hash);
    }
    return outStr;
}
