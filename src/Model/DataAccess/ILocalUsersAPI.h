//
// Created by thierrycd on 2023-08-31.
//

#pragma once

#include <Model/Models/LocalUsers.h>


/**
 * Interface for the local users API
 * This API is used to manage the local users infos
 * It is used to load, save and add new local users
 */
class ILocalUsersAPI
{
public:
    virtual void LoadLocalUserInfos() = 0;
    virtual void AddNewLocalUser(const LocalUserData& userData) = 0;
    virtual void UpdateLocalUser(const LocalUserData& userData) = 0;
    virtual void SaveLocalUserInfos() const = 0;

    [[nodiscard]] virtual const std::vector<LocalUserData>& GetLocalUserInfos() const = 0;
};
