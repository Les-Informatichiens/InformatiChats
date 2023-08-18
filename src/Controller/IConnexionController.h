//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/ViewModels/ConnexionViewModel.h"

class IConnexionController
{
public:
    virtual void AttemptConnectionWithUsername(const std::string &newUsername) = 0;
    virtual ConnexionViewModel getViewModel() = 0;
    virtual bool IsConnected() = 0;
};
