//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "Controller/IConfigController.h"
#include "IView.h"

#include <vector>


class ConfigView : public IView
{
public:
    explicit ConfigView(IConfigController& controller);
    ConfigView(std::vector<IPanel>& panels, IConfigController& controller);

    void SetController(IConfigController& controller);

    void Draw() const override;
    bool IsVisible() override;
    void AddPanel(IPanel& panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;

    IConfigController& controller;
};
