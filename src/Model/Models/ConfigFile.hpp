//
// Created by thierrycd on 2024-01-04.
//

#pragma once

#include "ConfigRefPair.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <nlohmann/json.hpp>


class ConfigFile
{
public:
    explicit ConfigFile(const std::string& configFileName)
        : valid(false)
    {
        this->input = std::ifstream(configFileName);
        if (this->Parse()) { this->valid = true; }
    }

    ~ConfigFile() { this->input.close(); }


    template<typename T>
    std::optional<T> LoadValue(const std::string& key)
    {
        if (!this->IsOpen()) { this->valid = false; }

        if (!this->valid) { return std::nullopt; }

        try { return data[key].get<T>(); } catch (const std::exception&)
        {
            std::cerr << "Error parsing data of type: " << typeid(T).name() << std::endl;
            return std::nullopt;
        }
    }

    template<typename... Ts>
    void LoadValues(ConfigRefPair<Ts>... key)
    {
        if (!this->IsOpen()) { this->valid = false; }

        if (!this->valid) { return; }

        firstVal(data, key...);
    }

    [[nodiscard]] bool IsOpen() const { return this->input.is_open(); }

private:
    bool Parse()
    {
        if (this->input.is_open())
        {
            try { data = nlohmann::json::parse(input); } catch (const std::exception&)
            {
                std::cerr << "Error parsing config file" << std::endl;
                return false;
            }

            return true;
        }

        return false;
    }

    template<typename T, typename... Ts>
    static void firstVal(nlohmann::ordered_json& data,
                         ConfigRefPair<T>& first,
                         ConfigRefPair<Ts>&... rest)
    {
        if (data.contains(first.key)) { first.value = data[first.key].template get<T>(); }

        if constexpr (sizeof...(rest) > 0) { firstVal(data, rest...); }
    }

    std::ifstream input;
    nlohmann::ordered_json data;
    bool valid;
};
