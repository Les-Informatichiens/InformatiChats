//
// Created by Jonathan Richard on 2023-12-29.
//

#pragma once
#include "IUserController.h"
#include "Model/ApplicationLogic/UserLogic.h"


class UserController : public IUserController
{
public:
    explicit UserController(UserLogic& userLogic_)
        : userLogic(userLogic_){};

    UserViewModel GetViewModel() override;
    void UpdateUserDisplayName(const std::string& displayName) override;
    void UpdateUserDescription(const std::string& description) override;
    void UpdateUserStatus(const std::string& status) override;
    void UpdateUserNameColor(const std::array<float, 3>& color) const override;

private:
    UserLogic& userLogic;
};
