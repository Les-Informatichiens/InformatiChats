//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IView.h"
#include <Controller/ILoginController.h>

#include <vector>
#include <functional>


class LoginView : public IView
{
public:
    explicit LoginView(ILoginController& controller);
    LoginView(std::vector<IPanel>& panels, ILoginController& controller);

    void SetController(ILoginController& controller);

    void Draw() const override;
    bool IsVisible() override;
    void AddPanel(IPanel& panel) override;

private:
    static constexpr int maxNameLength{32};
    std::vector<std::reference_wrapper<IPanel>> panels;

    ILoginController& controller;
};
