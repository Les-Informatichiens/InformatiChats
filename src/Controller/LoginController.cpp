//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"
#include "Model/Models/User.h"


LoginController::LoginController(Model& model_)
    : model(model_)
{
}

LoginViewModel LoginController::GetViewModel()
{
    return {model.GetMaxNameLength()};
}

void LoginController::LoginAttempt(const std::string& username_)
{
    model.LoginWithNewUser(username_);
}

bool LoginController::IsConnected()
{
    return model.IsClientConnected();
}
