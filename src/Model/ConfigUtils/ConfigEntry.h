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
        : name(std::move(name)), value(std::move(value)), description(std::move(description))
    {
    }

    template<typename... Validator>
    ConfigEntry(std::string name, ConfigType value, std::string description, Validator... validator)
        : name(std::move(name)), value(std::move(value)), description(std::move(description))
    {
        (AddValidator(validator), ...);
    }


    [[nodiscard]] const std::string& GetName() const;

    [[nodiscard]] const ConfigType& GetValue() const;
    void SetValue(const ConfigType& newVal_);

    [[nodiscard]] const std::vector<std::shared_ptr<Validator>>& GetValidators() const;
    [[nodiscard]] const std::string& GetDescription() const;
    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] std::string ValueToString() const;

    [[nodiscard]] bool Validate() const;
    [[nodiscard]] bool Validate(const ConfigType& newVal_) const;
    void AddValidator(const std::shared_ptr<Validator>& validator);

private:
    std::string name;
    ConfigType value;
    std::vector<std::shared_ptr<Validator>> validators;
    std::string description;
};
