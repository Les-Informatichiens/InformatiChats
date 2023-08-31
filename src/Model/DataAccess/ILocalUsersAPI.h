//
// Created by thierrycd on 2023-08-31.
//

#pragma once

#include <Model/Models/LocalUsers.h>


class ILocalUsersAPI
{
public:
    virtual void LoadLocalUserInfos() = 0;
    virtual void AddNewLocalUser(const UserData& userData) = 0;
    virtual void SaveLocalUserInfos() const = 0;

    [[nodiscard]] virtual const std::vector<UserData>& GetLocalUserInfos() const = 0;
};
