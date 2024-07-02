//
// Created by thierrycd on 2023-08-31.
//

#include "NlohmannJsonLocalUsersAPI.h"
#include <Model/Models/LocalUserData.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ranges>


/**
 * Load the local user infos from the users.json file
 */
void NlohmannJsonLocalUsersAPI::LoadLocalUserInfos()
{
    nlohmann::ordered_json data;
    std::ifstream input("users.json");
    if (input.is_open())
    {
        try
        {
            data = nlohmann::json::parse(input);
        } catch (const std::exception& e)
        {
        }
        input.close();
    }

    std::vector<LocalUserData> users;
    for (auto& [username, user]: data.items())
    {
        try
        {
            LocalUserData userData;
            userData.permanentUsername = username;
            userData.encryptedPassword = user["encryptedPassword"];
            userData.publicIdentificationKey = user["publicIdentificationKey"];
            userData.encryptedPrivateIdentificationKey = user["encryptedPrivateIdentificationKey"];
            userData.profile.displayName = user["profile"]["displayName"];
            userData.profile.description = user["profile"]["description"];
            userData.profile.status = user["profile"]["status"];
            userData.profile.nameColor = user["profile"]["nameColor"];

            std::unordered_map<std::string, ContactData> contacts;

            for (auto& jsonContact : user["contacts"])
            {
                ContactData contact;
                contact.permanentUsername = jsonContact["permanentUsername"];
                contact.publicIdentificationKey = jsonContact["publicIdentificationKey"];
                contact.lastSeen = jsonContact["lastSeen"];
                contact.profile.displayName = jsonContact["profile"]["displayName"];
                contact.profile.description = jsonContact["profile"]["description"];
                contact.profile.status = jsonContact["profile"]["status"];
                contact.profile.nameColor = jsonContact["profile"]["nameColor"];
                contacts.emplace(contact.publicIdentificationKey, std::move(contact));
            }
            userData.contacts = std::move(contacts);

            users.push_back(userData);
        }
        catch (const nlohmann::json::exception& e)
        {
            std::cout << "Error parsing user \"" << username << "\": "<< e.what() << std::endl;
            continue;
        }
    }
    this->localUserInfos = std::move(users);
}

/**
 * Save the local user infos to the users.json file
 */
void NlohmannJsonLocalUsersAPI::SaveLocalUserInfos() const
{
    nlohmann::ordered_json data;
    for (auto& user: this->localUserInfos)
    {
        nlohmann::ordered_json userJson;
        userJson["permanentUsername"] = user.permanentUsername;
        userJson["encryptedPassword"] = user.encryptedPassword;
        userJson["publicIdentificationKey"] = user.publicIdentificationKey;
        userJson["encryptedPrivateIdentificationKey"] = user.encryptedPrivateIdentificationKey;

        userJson["profile"]["displayName"] = user.profile.displayName;
        userJson["profile"]["description"] = user.profile.description;
        userJson["profile"]["status"] = user.profile.status;
        userJson["profile"]["nameColor"] = user.profile.nameColor;

        userJson["contacts"] = nlohmann::ordered_json::array();
        for (const auto& [permanentUsername, publicIdentificationKey, lastSeen, profile]: user.contacts | std::views::values)
        {
            nlohmann::ordered_json contactJson;
            contactJson["permanentUsername"] = permanentUsername;
            contactJson["publicIdentificationKey"] = publicIdentificationKey;
            contactJson["lastSeen"] = lastSeen;
            contactJson["profile"]["displayName"] = profile.displayName;
            contactJson["profile"]["description"] = profile.description;
            contactJson["profile"]["status"] = profile.status;
            contactJson["profile"]["nameColor"] = profile.nameColor;

            userJson["contacts"].push_back(contactJson);
        }

        data[user.permanentUsername] = userJson;
    }

    std::ofstream output("users.json");
    if (output.is_open())
    {
        output << data.dump(4);
        output.close();
    }
}

/**
 * Get the local user infos
 * @return The local user infos
 */
const std::vector<LocalUserData>& NlohmannJsonLocalUsersAPI::GetLocalUserInfos() const
{
    return this->localUserInfos;
}

/**
 * Add a new local user to the list
 * @param userData The user data to add
 */
void NlohmannJsonLocalUsersAPI::AddNewLocalUser(const LocalUserData& userData)
{
    this->localUserInfos.push_back(userData);
}

void NlohmannJsonLocalUsersAPI::UpdateLocalUser(const LocalUserData& updatedUserData)
{
    const auto userInfoIt = std::ranges::find_if(this->localUserInfos, [updatedUserData](const LocalUserData& userData) { return userData.publicIdentificationKey == updatedUserData.publicIdentificationKey; });
    if (userInfoIt == this->localUserInfos.end())
        return;

    *userInfoIt = updatedUserData;
}