//
// Created by thierrycd on 2023-09-16.
//

#pragma once

#include <string>
#include <unordered_map>


class IConfigAPI
{
public:
    virtual void LoadConfig() = 0;
    virtual void SaveConfig() const = 0;

    [[nodiscard]] virtual const std::unordered_map<std::string, std::string>& GetConfigs() const = 0;
    virtual void SetConfigs(const std::unordered_map<std::string, std::string>& configs) = 0;

    virtual ~IConfigAPI() = default;
};
