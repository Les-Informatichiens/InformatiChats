//
// Created by thierrycd on 2023-08-25.
//

#include "string_util.h"

namespace util {
    void ltrim(std::string& stringToTrim)
    {
        stringToTrim.erase(stringToTrim.begin(), std::find_if(stringToTrim.begin(), stringToTrim.end(), [](int ch) {
                               return !std::isspace(ch);
                           }));
    }

    void rtrim(std::string& stringToTrim)
    {
        stringToTrim.erase(std::find_if(stringToTrim.rbegin(), stringToTrim.rend(), [](int ch) {
                               return !std::isspace(ch);
                           }).base(),
                           stringToTrim.end());
    }

    void trim(std::string& stringToTrim)
    {
        ltrim(stringToTrim);
        rtrim(stringToTrim);
    }

    std::string ltrim_copy(std::string stringToTrim)
    {
        ltrim(stringToTrim);
        return stringToTrim;
    }

    std::string rtrim_copy(std::string stringToTrim)
    {
        rtrim(stringToTrim);
        return stringToTrim;
    }

    std::string trim_copy(std::string stringToTrim)
    {
        trim(stringToTrim);
        return stringToTrim;
    }
}// namespace util
