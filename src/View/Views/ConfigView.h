//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "IView.h"

#include <functional>
#include <vector>


class ConfigView final : public IView
{
public:
    ConfigView() = default;
    explicit ConfigView(std::vector<IPanel>& panels);

    void Draw() const override;
    bool IsVisible() override;
    void AddPanel(IPanel& panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
};
