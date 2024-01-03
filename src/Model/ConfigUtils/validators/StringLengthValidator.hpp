//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "Model/ConfigUtils/ConfigType.h"
#include "Validator.h"
#include <iostream>
#include <string>


class StringLengthValidator : public Validator
{
public:
    explicit StringLengthValidator(size_t max_) : min(0), max(max_) {}
    StringLengthValidator(size_t min_, size_t max_) : min(min_), max(max_) {}

    bool Validate(const ConfigType& value) override
    {
        if (!std::holds_alternative<std::string>(value))
        {
            std::cerr << "String length validator can only be used on strings" << std::endl;
            return false;
        }

        std::string val = std::get<std::string>(value);

        return val.length() >= min && val.length() <= max;
    }

private:
    size_t min;
    size_t max;
};
