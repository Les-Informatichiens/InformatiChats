//
// Created by Jean on 8/17/2023.
//

#pragma once

#include "Controller/IUserController.h"
#include "IPanel.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <ctime>
#include <glm/glm.hpp>

class UserInfoPanel : public IPanel
{
public:
    explicit UserInfoPanel(IUserController& controller);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    IUserController& controller;

    std::string displayNameBuf;
    std::string descriptionBuf;
    std::string statusBuf;
    float colorBuf[3];

};
