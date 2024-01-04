//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include <Model/ConfigUtils/IConfigGenerator.h>
#include <Model/DataAccess/IConfigAPI.h>
#include <Model/DataAccess/NlohmannJsonConfigAPI.h>
#include <Model/Models/ConfigSchema.h>

#include "Model/ConfigUtils/DefaultConfigGenerator.h"
#include <iostream>
#include <optional>


class ConfigLogic
{
public:
    ConfigLogic()
    {
        this->configSchema = ConfigSchema();
        this->configAPI = std::make_unique<NlohmannJsonConfigAPI>(NlohmannJsonConfigAPI());
        this->configGenerator = std::make_unique<DefaultConfigGenerator>(DefaultConfigGenerator());
    };

    template<typename T>
    [[nodiscard]] std::optional<T> GetConfigValue(const std::string& key) const;
    template<typename T>
    void SetConfigValue(const std::string& key, const T& value) const;

    void SetupConfig();
    void SaveConfig() const;
    [[nodiscard]] std::vector<ConfigEntry> GetConfigs() const;

    [[nodiscard]] bool IsCurrentlyEditingConfigs() const;
    void SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_);

    static ConfigLogic& GetInstance();

private:
    void GenerateConfig();
    void ApplyFilesConfig();
    [[nodiscard]] std::unordered_map<std::string, std::string> GetConfigMap() const;

private:
    ConfigSchema configSchema;
    std::unique_ptr<IConfigAPI> configAPI;
    std::unique_ptr<IConfigGenerator> configGenerator;

    bool currentlyEditingConfigs = false;
};
