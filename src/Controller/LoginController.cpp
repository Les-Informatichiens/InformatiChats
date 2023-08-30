//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"


LoginViewModel LoginController::GetViewModel()
{
    return {userLogic.GetMaxNameLength()};
}

void LoginController::LoginAttempt(const std::string& username_)
{
    userLogic.LoginWithNewUser(username_);
}

bool LoginController::IsConnected()
{
    return userLogic.IsClientConnected();
}
