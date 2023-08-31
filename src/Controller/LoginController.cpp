//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"


LoginViewModel LoginController::GetViewModel()
{
    return {userLogic.GetMaxNameLength()};
}

void LoginController::LoginAttempt(const std::string& username_, const std::string& password_)
{
    this->userLogic.LoginWithNewUser(username_, password_);
}

void LoginController::CreateUser(const std::string& username_, const std::string& password_)
{
    this->userLogic.CreateNewUser(username_, password_);
}

bool LoginController::IsConnected()
{
    return userLogic.IsClientConnected();
}
