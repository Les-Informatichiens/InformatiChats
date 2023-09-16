//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IPanel.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <Controller/ILoginController.h>
#include <util/string_util.h>

class LoginPanel : public IPanel
{
public:
    explicit LoginPanel(ILoginController& controller);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    ILoginController& controller;

    std::string selectedUsername;
    bool isLoggingIn = false;

    std::string usernameBuf;
    std::string passwordBuf;
    std::string passwordConfirmationBuf;
};
