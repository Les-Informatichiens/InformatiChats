//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "Controller/IConfigController.h"
#include "IPanel.h"

#include <string>


class ConfigPanel : public IPanel
{
public:
    explicit ConfigPanel(IConfigController& controller_);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    IConfigController& controller;
    std::string configText;
};
