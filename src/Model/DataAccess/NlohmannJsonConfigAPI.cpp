//
// Created by thierrycd on 2023-09-16.
//

#include "NlohmannJsonConfigAPI.h"

#include <fstream>
#include <nlohmann/json.hpp>


void NlohmannJsonConfigAPI::LoadConfig()
{
    nlohmann::ordered_json data;
    std::ifstream input("config.json");
    if (input.is_open())
    {
        try
        {
            data = nlohmann::json::parse(input);
        } catch (const std::exception& e)
        {
        }
        input.close();
    }

    this->config.clear();

    for (auto& [key, value]: data.items())
    {
        this->config[key] = value;
    }
}

void NlohmannJsonConfigAPI::SaveConfig() const
{
    nlohmann::ordered_json data;
    for (auto& [key, value]: this->config)
    {
        data[key] = value;
    }

    std::ofstream output("config.json");
    if (output.is_open())
    {
        output << data.dump(4);
        output.close();
    }
}

const std::unordered_map<std::string, std::string>& NlohmannJsonConfigAPI::GetConfigs() const
{
    return this->config;
}
