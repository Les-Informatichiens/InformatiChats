//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <array>
#include <string>

// TODO: not used yet

class UserProfile
{
    std::string displayName;
    std::string description;
    std::string status;
    std::array<uint8_t, 3> nameColor;
};

class UserData
{
    std::string username;
    std::string identificationKey;
    UserProfile profile;
};
