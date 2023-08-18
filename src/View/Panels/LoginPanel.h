//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/ILoginController.h"
#include "IPanel.h"

class LoginPanel : public IPanel
{
public:
    explicit LoginPanel(ILoginController &controller);

    void Draw() override;

    void Update() override;

    bool IsVisible() override;

private:
    ILoginController &controller;
};
