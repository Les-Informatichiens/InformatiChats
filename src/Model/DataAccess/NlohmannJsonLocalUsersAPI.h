//
// Created by thierrycd on 2023-08-31.
//

#pragma once

#include "ILocalUsersAPI.h"
#include <Model/Models/LocalUserData.h>
#include <vector>


/**
 * Implementation of the ILocalUsersAPI using nlohmann::json
 */
class NlohmannJsonLocalUsersAPI : public ILocalUsersAPI
{
public:
    NlohmannJsonLocalUsersAPI(){};

    void LoadLocalUserInfos() override;
    void AddNewLocalUser(const LocalUserData& userData) override;
    void SaveLocalUserInfos() const override;
    void UpdateLocalUser(const LocalUserData& userData) override;

    [[nodiscard]] const std::vector<LocalUserData>& GetLocalUserInfos() const override;

private:
    std::vector<LocalUserData> localUserInfos;
};
