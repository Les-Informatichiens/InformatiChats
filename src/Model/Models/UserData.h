//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <array>
#include <string>


/**
 * Represents the profile of a user, which is the info that will be sent to other users.
 */
struct UserProfile
{
    std::string displayName;
    std::string description;
    std::string status;
    std::array<uint8_t, 3> nameColor;
};

/**
 * Represents the data of a user, which is the info that will be stored locally.
 */
struct UserData
{
    std::string permanentUsername;
    std::string encryptedPassword;
    std::string publicIdentificationKey;
    std::string encryptedPrivateIdentificationKey;
    UserProfile profile;
};
