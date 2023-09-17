//
// Created by fofi1 on 2023-09-16.
//

#pragma once

#include "IConfigAPI.h"


class NlohmannJsonConfigAPI : public IConfigAPI
{
public:
    void LoadConfig() override;
    void SaveConfig() const override;

    [[nodiscard]] const std::unordered_map<std::string, std::string>& GetConfig() const override;

private:
    std::unordered_map<std::string, std::string> config;
};
