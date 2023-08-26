//
// Created by thierrycd on 2023-08-25.
//

#pragma once

#include <algorithm>
#include <string>

namespace util {
    void ltrim(std::string& stringToTrim);
    void rtrim(std::string& stringToTrim);
    void trim(std::string& stringToTrim);

    std::string ltrim_copy(std::string stringToTrim);
    std::string rtrim_copy(std::string stringToTrim);
    std::string trim_copy(std::string stringToTrim);
}// namespace util
