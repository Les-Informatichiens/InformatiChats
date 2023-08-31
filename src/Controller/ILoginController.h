//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Controller/ViewModels/LoginViewModel.h>


class ILoginController
{
public:
    virtual LoginViewModel GetViewModel() = 0;

    virtual void LoginAttempt(const std::string& username, const std::string& password) = 0;
    virtual void CreateUser(const std::string& username, const std::string& password) = 0;
    [[nodiscard]] virtual bool IsConnected() const = 0;
    [[nodiscard]] virtual bool IsCreatingNewUser() const = 0;
    virtual void SetCreatingNewUser(bool createNewUser) = 0;
    virtual void LoadLocalUsers() const = 0;
};
