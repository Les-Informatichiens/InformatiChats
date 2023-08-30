//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "ILoginController.h"
#include "Model/ApplicationLogic/UserLogic.h"
#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include <Controller/ViewModels/LoginViewModel.h>


class LoginController : public ILoginController
{
public:
    explicit LoginController(UserLogic& userLogic) : userLogic(userLogic){};

    LoginViewModel GetViewModel() override;
    void LoginAttempt(const std::string& username) override;
    bool IsConnected() override;

private:
    UserLogic& userLogic;
};
