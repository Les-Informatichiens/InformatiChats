//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include <Model/ConfigUtils/IConfigGenerator.h>
#include <Model/DataAccess/IConfigAPI.h>
#include <Model/Models/ConfigSchema.h>

#include <iostream>
#include <optional>


class ConfigLogic
{
public:
    ConfigLogic(ConfigSchema& configSchema_, IConfigAPI& configAPI, IConfigGenerator& configGenerator)
        : configSchema(configSchema_), configAPI(configAPI), configGenerator(configGenerator){};

    template<typename T>
    [[nodiscard]] std::optional<T> GetConfigValue(const std::string& key) const;
    template<typename T>
    void SetConfigValue(const std::string& key, const T& value) const;

    void SetupConfig() const;
    [[nodiscard]] std::vector<ConfigEntry> GetConfigs() const;
    [[nodiscard]] bool IsCurrentlyEditingConfigs() const;
    void SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_);

private:
    void GenerateConfig() const;
    void ApplyFilesConfig() const;

private:
    ConfigSchema& configSchema;
    IConfigAPI& configAPI;
    IConfigGenerator& configGenerator;

    bool currentlyEditingConfigs = false;
};
