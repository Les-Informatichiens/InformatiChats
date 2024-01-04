//
// Created by thierrycd on 2024-01-03.
//

#include "ConfigLogic.h"

void ConfigLogic::SetupConfig() const
{
    this->GenerateConfig();
    this->configAPI.LoadConfig();
    this->ApplyFilesConfig();
}

template<typename T>
std::optional<T> ConfigLogic::GetConfigValue(const std::string& key) const
{
    auto it = std::find_if(this->configSchema.entries.begin(), this->configSchema.entries.end(),
                           [&key](const ConfigEntry& entry) { return entry.GetName() == key; });

    if (it == this->configSchema.entries.end())
    {
        std::cerr << "Config entry " << key << " not found" << std::endl;
        return std::nullopt;
    }

    ConfigType val = it->GetValue();

    if (!std::holds_alternative<T>(val))
    {
        std::cerr << "Config entry " << key << " is not of type " << typeid(T).name() << std::endl;
        return std::nullopt;
    }

    return std::get<T>(val);
}

template<typename T>
void ConfigLogic::SetConfigValue(const std::string& key, const T& value) const
{
    auto it = std::find_if(this->configSchema.entries.begin(), this->configSchema.entries.end(),
                           [&key](const ConfigEntry& entry) { return entry.GetName() == key; });

    if (it == this->configSchema.entries.end())
    {
        std::cerr << "Config entry " << key << " not found" << std::endl;
        return;
    }

    ConfigType val = it->GetValue();

    if (!std::holds_alternative<T>(val))
    {
        std::cerr << "Config entry " << key << " is not of type " << typeid(T).name() << std::endl;
        return;
    }

    it->SetValue(value);
}

std::vector<ConfigEntry> ConfigLogic::GetConfigs() const
{
    return this->configSchema.entries;
}

bool ConfigLogic::IsCurrentlyEditingConfigs() const
{
    return this->currentlyEditingConfigs;
}

void ConfigLogic::SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_)
{
    this->currentlyEditingConfigs = currentlyEditingConfigs_;
}

void ConfigLogic::GenerateConfig() const
{
    this->configSchema.entries = this->configGenerator.GenerateConfig();
}

void ConfigLogic::ApplyFilesConfig() const
{
    //try to set the value of each config entry generated by the config generator
    //to the value stored in the configAPI, if the values in the config API are not
    //valid, keep the generated value
}