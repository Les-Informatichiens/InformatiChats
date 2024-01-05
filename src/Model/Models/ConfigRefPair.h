//
// Created by fofi1 on 2024-01-04.
//

#pragma once

#include <string>
#include <utility>

template<typename T>
struct ConfigRefPair
{
    ConfigRefPair(std::string  key_, T& value_)
        : key(std::move(key_)), value(value_) {}

    std::string key;
    T& value;
};

template<typename T>
struct ConfigRefPairWithDefault
{
    ConfigRefPairWithDefault(std::string  key_, T& value_, T defaultValue_)
            : key(std::move(key_)), value(value_), defaultValue(defaultValue_) {}

    std::string key;
    T& value;
    T defaultValue;
};