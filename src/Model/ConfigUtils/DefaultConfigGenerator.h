//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "ConfigEntry.h"
#include "IConfigGenerator.h"
#include "Model/ConfigUtils/validators/RangeValidator.hpp"
#include "Model/ConfigUtils/validators/StringLengthValidator.hpp"
#include "Model/ConfigUtils/validators/Validator.h"

#include <map>


class DefaultConfigGenerator : public IConfigGenerator
{
public:
    [[nodiscard]] std::vector<ConfigEntry> GenerateConfig() const override
    {
        std::vector<ConfigEntry> configSchema;
        ConfigEntry volume = ConfigEntry("VolumeLevel",
                                         50,
                                         "The volume of the sound. 0 is mute, 100 is max volume.",
                                         std::make_shared<IntRangeValidator>(0, 100));
        configSchema.push_back(std::move(volume));

        return std::move(configSchema);
    }
};
