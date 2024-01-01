//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "Model/ConfigUtils/ConfigType.h"
#include "Validator.h"
#include <iostream>


class IntRangeValidator : public Validator
{
public:
    IntRangeValidator(int min_, int max_) : min(min_), max(max_) {}

    bool validate(const ConfigType& value) override
    {
        if (!std::holds_alternative<int>(value))
        {
            std::cerr << "Int range validator can only be used on ints" << std::endl;
            return false;
        }

        int val = std::get<int>(value);

        return val >= min && val <= max;
    }

private:
    int min;
    int max;
};


class DoubleRangeValidator : public Validator
{
public:
    DoubleRangeValidator(double min_, double max_) : min(min_), max(max_) {}

    bool validate(const ConfigType& value) override
    {
        if (!std::holds_alternative<double>(value))
        {
            std::cerr << "Double range validator can only be used on doubles" << std::endl;
            return false;
        }

        double val = std::get<double>(value);

        return val >= min && val <= max;
    }

private:
    double min;
    double max;
};


class FloatRangeValidator : public Validator
{
public:
    FloatRangeValidator(float min_, float max_) : min(min_), max(max_) {}

    bool validate(const ConfigType& value) override
    {
        if (!std::holds_alternative<float>(value))
        {
            std::cerr << "Float range validator can only be used on floats" << std::endl;
            return false;
        }

        float val = std::get<float>(value);

        return val >= min && val <= max;
    }

private:
    float min;
    float max;
};
