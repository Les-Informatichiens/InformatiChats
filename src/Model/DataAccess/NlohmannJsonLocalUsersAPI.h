//
// Created by thierrycd on 2023-08-31.
//

#pragma once

#include "ILocalUsersAPI.h"
#include <Model/Models/UserData.h>
#include <vector>


/**
 * Implementation of the ILocalUsersAPI using nlohmann::json
 */
class NlohmannJsonLocalUsersAPI : public ILocalUsersAPI
{
public:
    NlohmannJsonLocalUsersAPI(){};

    void LoadLocalUserInfos() override;
    void AddNewLocalUser(const UserData& userData) override;
    void SaveLocalUserInfos() const override;
    void UpdateLocalUser(const UserData& userData) override;

    [[nodiscard]] const std::vector<UserData>& GetLocalUserInfos() const override;

private:
    std::vector<UserData> localUserInfos;
};
