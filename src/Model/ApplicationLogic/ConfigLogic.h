//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


class ConfigLogic
{
public:
    template<typename T>
    static void SaveConfigValue(const std::string& key, const T& value)
    {
        std::ifstream input("config.json");
        if (!input.is_open())
        {
            std::ofstream output("config.json");
            output << "{}";
            output.close();
        }
        input = std::ifstream("config.json");

        nlohmann::ordered_json data;

        if (input.is_open())
        {
            try { data = nlohmann::json::parse(input); } catch (const std::exception& e)
            {
                std::cerr << "Error parsing config file" << std::endl;
                return;
            }

            input.close();
        }

        std::ofstream output("config.json");
        if (output.is_open())
        {
            data[key] = value;

            output << data.dump(4);
            output.close();
        }
    }

    template<typename T>
    static std::optional<T> LoadConfigValue(const std::string& key)
    {
        if (std::ifstream input("config.json");
            input.is_open())
        {
            nlohmann::ordered_json data;

            try { data = nlohmann::json::parse(input); } catch (const std::exception& e)
            {
                std::cerr << "Error parsing config file" << std::endl;
                return std::nullopt;
            }

            input.close();

            try { return data[key].get<T>(); } catch (const std::exception& e)
            {
                std::cerr << "Error parsing data of type: " << typeid(T) << std::endl;
                return std::nullopt;
            }
        }

        return std::nullopt;
    }

private:
    inline static std::string configFilePath{"config.json"};
};
