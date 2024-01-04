//
// Created by fofi1 on 2024-01-04.
//

#pragma once

#include <string>

template<typename T>
struct ConfigRefPair
{
    ConfigRefPair(const std::string& key_, T& value_)
        : key(key_), value(value_) {}

    std::string key;
    T& value;
};
