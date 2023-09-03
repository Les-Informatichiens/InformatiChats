//
// Created by thierrycd on 2023-08-31.
//

#include "NlohmannJsonLocalUsersAPI.h"
#include <Model/Models/UserData.h>
#include <fstream>
#include <nlohmann/json.hpp>


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

    std::vector<UserData> users;
    for (auto& [username, user]: data.items())
    {
        UserData userData;
        userData.permanentUsername = username;
        userData.encryptedPassword = user["encryptedPassword"];
        userData.publicIdentificationKey = user["publicIdentificationKey"];
        userData.encryptedPrivateIdentificationKey = user["encryptedPrivateIdentificationKey"];
        users.push_back(userData);
    }
    this->localUserInfos = std::move(users);
}

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
        data[user.permanentUsername] = userJson;
    }

    std::ofstream output("users.json");
    if (output.is_open())
    {
        output << data.dump(4);
        output.close();
    }
}

const std::vector<UserData>& NlohmannJsonLocalUsersAPI::GetLocalUserInfos() const
{
    return this->localUserInfos;
}
void NlohmannJsonLocalUsersAPI::AddNewLocalUser(const UserData& userData)
{
    this->localUserInfos.push_back(userData);
}
