//
// Created by Jonathan Richard on 2024-01-04.
//

#include "UUID.h"

#include <iostream>
#include <openssl/rand.h>

namespace UUID {

    std::string generateUUID()
    {
        unsigned char uuid[16]; // UUIDs are 16 bytes (128 bits) long

        // Generate random UUID
        if (RAND_bytes(uuid, sizeof(uuid)) != 1) {
            std::cerr << "Error generating random UUID" << std::endl;
            return "";
        }

        // Set UUID version (Version 4: Random)
        uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4

        // Set UUID variant (Reserved bits)
        uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant bits

        // Format UUID in hexadecimal string
        char uuidStr[36];
        snprintf(uuidStr, sizeof(uuidStr),
                 "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                 uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7],
                 uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

        return std::string(uuidStr);
    }
}