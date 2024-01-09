//
// Created by Jonathan Richard on 2023-12-29.
//

#pragma once


//
// Created by Jean on 8/18/2023.
//

#pragma once
#include "ViewModels/UserViewModel.h"


class IUserController
{
public:
    virtual UserViewModel GetViewModel() = 0;

    virtual void UpdateUserDisplayName(const std::string& displayName) = 0;

    virtual void UpdateUserDescription(const std::string& description) = 0;

    virtual void UpdateUserStatus(const std::string& status) = 0;

    virtual void UpdateUserNameColor(const std::array<float, 3>& color) const = 0;

};
