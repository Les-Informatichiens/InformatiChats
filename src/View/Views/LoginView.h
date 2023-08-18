//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/ILoginController.h"
#include "IView.h"
#include <vector>

class LoginView : public IView
{
public:
    explicit LoginView(ILoginController &controller);
    LoginView(std::vector<IPanel> &panels, ILoginController &controller);
    void Draw() const override;

    void AddPanel(IPanel &panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;

    ILoginController &controller;
};