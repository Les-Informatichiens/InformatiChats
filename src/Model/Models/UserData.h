//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <array>
#include <string>

// TODO: not used yet

struct UserProfile
{
    std::string displayName;
    std::string description;
    std::string status;
    std::array<uint8_t, 3> nameColor;
};

struct UserData
{
    std::string permanentUsername;
    std::string encryptedPassword;
    std::string publicIdentificationKey;
    std::string encryptedPrivateIdentificationKey;
    //UserProfile profile;
};
