//
// Created by thierrycd on 2024-01-03.
//

#include "ConfigEntry.h"
#include "util/VariantUtil.h"

#include <iostream>


void ConfigEntry::AddValidator(const std::shared_ptr<Validator>& validator)
{
    this->validators.push_back(validator);
}

const std::string& ConfigEntry::GetName() const
{
    return this->name;
}

const ConfigType& ConfigEntry::GetValue() const
{
    return this->value;
}

const std::vector<std::shared_ptr<Validator>>& ConfigEntry::GetValidators() const
{
    return this->validators;
}

const std::string& ConfigEntry::GetDescription() const
{
    return this->description;
}

std::string ConfigEntry::ToString() const
{
    std::string val;

    if (std::holds_alternative<int>(value))
    {
        val = std::to_string(std::get<int>(value));
    }
    else if (std::holds_alternative<double>(value))
    {
        val = std::to_string(std::get<double>(value));
    }
    else if (std::holds_alternative<float>(value))
    {
        val = std::to_string(std::get<float>(value));
    }
    else if (std::holds_alternative<std::string>(value))
    {
        val = std::get<std::string>(value);
    }
    else if (std::holds_alternative<bool>(value))
    {
        val = std::to_string(std::get<bool>(value));
    }

    return name + "= " + val + ": " + description + "\n";
}

std::string ConfigEntry::ValueToString() const
{
    std::string val;

    if (std::holds_alternative<int>(value))
    {
        val = std::to_string(std::get<int>(value));
    }
    else if (std::holds_alternative<double>(value))
    {
        val = std::to_string(std::get<double>(value));
    }
    else if (std::holds_alternative<float>(value))
    {
        val = std::to_string(std::get<float>(value));
    }
    else if (std::holds_alternative<std::string>(value))
    {
        val = std::get<std::string>(value);
    }
    else if (std::holds_alternative<bool>(value))
    {
        val = std::to_string(std::get<bool>(value));
    }
    return val;
}

bool ConfigEntry::Validate() const
{
    for (const auto& validator: this->validators)
    {
        if (!validator->Validate(value))
        {
            return false;
        }
    }

    return true;
}

bool ConfigEntry::Validate(const ConfigType& newVal_) const
{
    for (const auto& validator: this->validators)
    {
        if (!validator->Validate(newVal_))
        {
            return false;
        }
    }

    return true;
}

void ConfigEntry::SetValue(const ConfigType& newVal_)
{
    if (!hold_same_type(newVal_, this->value))
    {
        std::cerr << "New value for config entry " << this->name << " is not of the same type." << std::endl;
        return;
    }

    if (!Validate(newVal_))
    {
        std::cerr << "New value for config entry " << this->name << " is invalid." << std::endl;
        return;
    }

    this->value = newVal_;
}
