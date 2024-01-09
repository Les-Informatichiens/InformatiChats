//
// Created by Jonathan Richard on 2023-12-29.
//

#pragma once

#include <array>
#include <string>

struct UserViewModel
{
    std::string username;
    std::string displayName;
    std::string description;
    std::string status;
    std::array<float, 3> nameColor;
};
