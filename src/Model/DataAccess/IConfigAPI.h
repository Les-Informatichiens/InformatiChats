//
// Created by fofi1 on 2023-09-16.
//

#pragma once

#include <string>
#include <unordered_map>


class IConfigAPI
{
public:
    virtual void LoadConfig() = 0;
    virtual void SaveConfig() const = 0;

    [[nodiscard]] virtual const std::unordered_map<std::string, std::string>& GetConfig() const = 0;
};
