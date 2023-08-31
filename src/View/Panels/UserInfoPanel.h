//
// Created by Jean on 8/17/2023.
//

#pragma once

#include "IPanel.h"
#include "imgui.h"
#include <Controller/IChannelController.h>
#include <ctime>
#include <glm/glm.hpp>

class UserInfoPanel : public IPanel
{
public:
    explicit UserInfoPanel(IChannelController& controller);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    IChannelController& controller;
};
