//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IView.h"

#include <functional>
#include <vector>


class LoginView final : public IView
{
public:
    LoginView() = default;
    explicit LoginView(std::vector<IPanel>& panels);

    void Draw() const override;
    bool IsVisible() override;
    void AddPanel(IPanel& panel) override;

private:
    static constexpr int maxNameLength{32};
    std::vector<std::reference_wrapper<IPanel>> panels;
};
