//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include <Model/ConfigUtils/ConfigType.h>
#include <Model/ConfigUtils/validators/Validator.h>

#include <memory>
#include <string>
#include <variant>
#include <vector>


class ConfigEntry
{
public:
    ConfigEntry(std::string name, ConfigType value, std::string description)
        : name(std::move(name)), value(std::move(value)), description(std::move(description)), validators()
    {
    }

    template<typename... Validator>
    ConfigEntry(std::string name, ConfigType value, std::string description, Validator... validator)
        : name(std::move(name)), value(std::move(value)), description(std::move(description)), validators()
    {
        (addValidator(validator), ...);
    }

    void addValidator(const std::shared_ptr<Validator>& validator)
    {
        validators.push_back(validator);
    }

    [[nodiscard]] const std::string& GetName() const
    {
        return name;
    }

    [[nodiscard]] const ConfigType& GetValue() const
    {
        return value;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<Validator>>& GetValidators() const
    {
        return validators;
    }

    [[nodiscard]] const std::string& GetDescription() const
    {
        return description;
    }

    [[nodiscard]] std::string toString() const
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

    [[nodiscard]] bool validate() const
    {
        for (const auto& validator: this->validators)
        {
            if (!validator->validate(value))
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool validate(const ConfigType& newVal_) const
    {
        for (const auto& validator: this->validators)
        {
            if (!validator->validate(newVal_))
            {
                return false;
            }
        }

        return true;
    }

private:
    std::string name;
    ConfigType value;
    std::vector<std::shared_ptr<Validator>> validators;
    std::string description;
};
