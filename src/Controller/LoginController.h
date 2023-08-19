//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "ILoginController.h"
#include <Controller/ViewModels/LoginViewModel.h>

#include "Model/Model.h"
#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"


class LoginController : public ILoginController
{
public:
    explicit LoginController(Model& model);

    LoginViewModel GetViewModel() override;

    void LoginAttempt(const std::string& username);

    bool IsConnected();

private:

    Model& model;
};
