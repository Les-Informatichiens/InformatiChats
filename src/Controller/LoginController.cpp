//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"


LoginViewModel LoginController::GetViewModel()
{
    return {userLogic.GetMaxNameLength()};
}

void LoginController::LoginAttempt(const std::string& username_, const std::string& password)
{
    userLogic.LoginWithNewUser(username_, password);
}

void LoginController::CreateUser(const std::string& username, const std::string& password)
{
    model.CreateNewUser(username, password);
}

bool LoginController::IsConnected()
{
    return userLogic.IsClientConnected();
}