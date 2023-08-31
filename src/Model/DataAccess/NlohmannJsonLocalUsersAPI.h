//
// Created by thierrycd on 2023-08-31.
//

#pragma once

#include "ILocalUsersAPI.h"
#include <Model/Models/UserData.h>
#include <vector>


class NlohmannJsonLocalUsersAPI : public ILocalUsersAPI
{
public:
    NlohmannJsonLocalUsersAPI() : localUserInfos(){};

    void LoadLocalUserInfos() override;
    void AddNewLocalUser(const UserData& userData) override;
    void SaveLocalUserInfos() const override;

    [[nodiscard]] const std::vector<UserData>& GetLocalUserInfos() const override;

private:
    std::vector<UserData> localUserInfos;
};
