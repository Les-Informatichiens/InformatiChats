//
// Created by thierrycd on 2024-01-04.
//

#pragma once

#include "ConfigRefPair.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <utility>

namespace config {
    static inline const std::string CONFIG_FILE_NAME = "config.json";
}


class ConfigFile
{
public:
    explicit ConfigFile(std::string configFileName)
        : fileName(std::move(configFileName)), valid(false)
    {
        this->input = std::fstream(this->fileName, std::ios::in | std::ios::out | std::ios::ate);
        if (!this->input.is_open())
        {
            std::ofstream file(this->fileName);
            file << "{}";
            file.close();
            this->input = std::fstream(this->fileName, std::ios::in | std::ios::out | std::ios::ate);
            this->data = nlohmann::ordered_json();
        }

        if (this->Parse()) { this->valid = true; }
    }

    ~ConfigFile() { this->Close(); }

    template<typename T>
    void SaveValue(const std::string& key, const T& value)
    {
        if (!this->IsOpen()) { this->valid = false; }

        if (!this->valid) { return; }

        data[key] = value;

        //Overwrite the file with the new data
        this->input.seekg(0);
        this->input << data.dump(4);
        this->input.flush();

        // Truncate the file by writing an empty buffer
        // This is needed because the new data might be smaller than the old data
        this->input.write("", 0);
    }


    /**
     * \brief Saves a value from the given ConfigRefPair
     * \tparam T
     * \param pair
     * \return
     */
    template<typename T>
    ConfigFile& operator<<(const ConfigRefPair<T>& pair)
    {
        this->SaveValue(pair.key, pair.value);
        return *this;
    }

    template<typename T>
    std::optional<T> LoadValue(const std::string& key)
    {
        if (!this->IsOpen()) { this->valid = false; }

        if (!this->valid) { return std::nullopt; }

        try
        {
            return data[key].get<T>();
        } catch (const std::exception&)
        {
            std::cerr << "-- Error parsing data of type of key `" << key << "`: " << typeid(T).name() << std::endl;
            std::cerr << "   Expected type: " << data[key].type_name() << std::endl;
            return std::nullopt;
        }
    }

    template<typename... Ts>
    void LoadValues(ConfigRefPair<Ts>... key)
    {
        if (!this->IsOpen()) { this->valid = false; }

        if (!this->valid) { return; }

        LoadValuesRecursive(key...);
    }

    template<typename... Ts>
    void LoadValuesWithDefault(ConfigRefPairWithDefault<Ts>... key)
    {
        if (!this->IsOpen()) { this->valid = false; }

        if (!this->valid) { return; }

        LoadValuesWithDefaultRecursive(key...);
    }

    /**
     * \brief Loads a value into the given ConfigRefPair
     *        If the value does not exist, the value is not changed.
     * \tparam T
     * \param pair
     * \return
     */
    template<typename T>
    ConfigFile& operator>>(const ConfigRefPair<T>& pair)
    {
        if (auto loadedValue = this->LoadValue<T>(pair.key);
            loadedValue.has_value())
        {
            pair.value = loadedValue.value();
        }

        return *this;
    }

    /**
     * \brief Loads a value into the given ConfigRefPairWithDefault.
     *        If the value does not exist, the default value is used.
     * \tparam T
     * \param pair
     * \return
     */
    template<typename T>
    ConfigFile& operator>>(const ConfigRefPairWithDefault<T>& pair)
    {
        auto loadedValue = this->LoadValue<T>(pair.key);
        pair.value = loadedValue.value_or(pair.defaultValue);

        return *this;
    }

    [[nodiscard]] bool IsOpen() const { return this->input.is_open(); }

    void Close()
    {
        this->input.close();
        this->valid = false;
        this->data = nlohmann::ordered_json();
        this->fileName = "";
    }

private:
    bool Parse()
    {
        if (this->input.is_open())
        {
            this->input.seekg(0);
            try
            {
                data = nlohmann::json::parse(this->input);
            } catch (const std::exception&)
            {
                std::cerr << "-- Error parsing config file" << std::endl;
                return false;
            }

            return true;
        }

        return false;
    }

    template<typename T, typename... Ts>
    void LoadValuesRecursive(ConfigRefPair<T>& first,
                             ConfigRefPair<Ts>&... rest)
    {
        if (data.contains(first.key))
        {
            try
            {
                first.value = data[first.key].template get<T>();
            } catch (const std::exception&)
            {
                std::cerr << "-- Error parsing data of type of key `" << first.key << "`: " << typeid(T).name();
                std::cerr << "\n   Expected type: " << data[first.key].type_name() << std::endl;
            }
        }

        if constexpr (sizeof...(rest) > 0) { LoadValuesRecursive(rest...); }
    }

    template<typename T, typename... Ts>
    void LoadValuesWithDefaultRecursive(ConfigRefPairWithDefault<T>& first,
                                        ConfigRefPairWithDefault<Ts>&... rest)
    {
        if (data.contains(first.key))
        {
            try
            {
                first.value = data[first.key].template get<T>();
            } catch (const std::exception&)
            {
                std::cerr << "-- Error parsing data of type of key `" << first.key << "`: " << typeid(T).name();
                std::cerr << "\n   Expected type: " << data[first.key].type_name() << std::endl;
                std::cerr << "   Using default value: " << first.defaultValue << std::endl;
                first.value = first.defaultValue;
            }
        }
        else
        {
            first.value = first.defaultValue;
        }

        if constexpr (sizeof...(rest) > 0) { LoadValuesWithDefaultRecursive(rest...); }
    }

private:
    std::fstream input;
    nlohmann::ordered_json data;
    bool valid;

    std::string fileName;
};
