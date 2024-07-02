//
// Created by Jonathan Richard on 2023-08-27.
//

#pragma once

#include <array>
#include <unordered_map>
#include <string>
#include <vector>


/**
 * Represents the profile of a user, which is the info that will be sent to other users.
 */
struct UserProfile
{
    std::string displayName;
    std::string description;
    std::string status;
    std::array<float, 3> nameColor;
};

struct ContactData
{
    std::string permanentUsername;
    std::string publicIdentificationKey;
    uint64_t lastSeen{};
    UserProfile profile;
};

/**
 * Represents the data of a user, which is the info that will be stored locally.
 */
struct LocalUserData
{
    std::string permanentUsername;
    std::string encryptedPassword;
    std::string publicIdentificationKey;
    std::string encryptedPrivateIdentificationKey;
    UserProfile profile;
    std::unordered_map<std::string, ContactData> contacts;
};
