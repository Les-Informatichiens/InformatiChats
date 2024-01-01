//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "ConfigEntry.h"
#include <map>
#include <string>
#include <vector>

class IConfigGenerator
{
public:
    [[nodiscard]] virtual std::vector<ConfigEntry> GenerateConfig() const = 0;
};
