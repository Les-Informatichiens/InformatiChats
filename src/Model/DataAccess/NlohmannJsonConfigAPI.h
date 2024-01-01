//
// Created by thierrycd on 2023-09-16.
//

#pragma once

#include "IConfigAPI.h"


class NlohmannJsonConfigAPI : public IConfigAPI
{
public:
    void LoadConfig() override;
    void SaveConfig() const override;

    [[nodiscard]] const std::unordered_map<std::string, std::string>& GetConfigs() const override;

private:
    std::unordered_map<std::string, std::string> config;
};
