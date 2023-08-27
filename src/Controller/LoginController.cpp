//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"
#include "Model/User.h"


LoginController::LoginController(Model& model_)
    : model(model_)
{
}

LoginViewModel LoginController::GetViewModel()
{
    return {};
}

void LoginController::LoginAttempt(const std::string& username_, const std::string& password)
{
    model.LoginWithNewUser(username_, password);
}

void LoginController::CreateUser(const std::string& username, const std::string& password)
{
    model.CreateNewUser(username, password);
}

bool LoginController::IsConnected()
{
    return model.IsClientConnected();
}