//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include <string>
#include <variant>

typedef std::variant<int, double, float, std::string, bool> ConfigType;
