//
// Created by Jonathan Richard on 2023-09-30.
//

#pragma once

#include <string>

namespace Hashing
{
    std::string SHA256(const char* key, const char* data, unsigned char*& out);
}
