//
// Created by Jean on 8/17/2023.
//
#pragma once
#include "IPanel.h"
#include <Model/Chat.h>

class UserInfoPanel : public IPanel {
public:
    explicit UserInfoPanel(Chat &chatClient);

    void Draw() override;

    void Update() override;

    bool IsVisible() override;

    Chat& chatClient;
};