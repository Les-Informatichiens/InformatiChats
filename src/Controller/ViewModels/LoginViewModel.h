//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Model/Models/LocalUserData.h>
#include <string>
#include <vector>


struct LoginViewModel
{
    size_t maxNameLength;
    std::vector<LocalUserData> localUserInfos;
};
