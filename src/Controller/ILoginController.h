//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/ViewModels/LoginViewModel.h"

class ILoginController
{
public:
    virtual void AttemptConnectionWithUsername(const std::string &newUsername) = 0;
    virtual LoginViewModel getViewModel() = 0;
    virtual bool IsConnected() = 0;
};
