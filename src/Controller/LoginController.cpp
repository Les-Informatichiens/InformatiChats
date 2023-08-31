//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"


LoginViewModel LoginController::GetViewModel()
{
    return {this->userLogic.GetMaxNameLength(), this->userLogic.GetLocalUserInfos()};
}

void LoginController::LoginAttempt(const std::string& username_, const std::string& password_)
{
    this->userLogic.LoginWithNewUser(username_, password_);
}

void LoginController::CreateUser(const std::string& username_, const std::string& password_)
{
    this->userLogic.CreateNewUser(username_, password_);
}

bool LoginController::IsConnected() const
{
    return userLogic.IsClientConnected();
}

bool LoginController::IsCreatingNewUser() const
{
    return this->createNewUser;
}

void LoginController::SetCreatingNewUser(bool createNewUser_)
{
    this->createNewUser = createNewUser_;
}

void LoginController::LoadLocalUsers() const
{
    this->userLogic.LoadLocalUsers();
}
