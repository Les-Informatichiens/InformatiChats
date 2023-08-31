//
// Created by thierrycd on 2023-08-30.
//

#pragma once

#include "IPanel.h"
#include <Controller/ILoginController.h>

class CreateNewUserPanel : public IPanel
{
public:
    explicit CreateNewUserPanel(ILoginController& controller);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    ILoginController& controller;

    std::string usernameBuf;
    std::string passwordBuf;
    std::string passwordConfirmationBuf;
};
