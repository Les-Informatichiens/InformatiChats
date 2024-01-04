//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "ILoginController.h"
#include "Model/ApplicationLogic/ConfigLogic.h"
#include <Model/ApplicationLogic/Command/CommandManager.h>
#include <Model/ApplicationLogic/UserLogic.h>


class LoginController : public ILoginController
{
public:
    explicit LoginController(UserLogic& userLogic, CommandManager& commandManager)
        : userLogic(userLogic), commandManager(commandManager){};

    LoginViewModel GetViewModel() override;

    void LoginAttempt(const std::string& username, const std::string& password) override;
    void CreateUser(const std::string& username, const std::string& password) override;
    void LoadLocalUsers() const override;

    [[nodiscard]] bool IsConnected() const override;
    [[nodiscard]] bool IsCreatingNewUser() const override;
    void SetCreatingNewUser(bool createNewUser) override;

private:
    UserLogic& userLogic;
    CommandManager& commandManager;

    bool createNewUser = false;
};
