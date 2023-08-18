//
// Created by Jean on 8/17/2023.
//
#pragma once
#include "../../Controller/IChannelController.h"
#include "../../Model/Chat.h"
#include "IPanel.h"

class UserInfoPanel : public IPanel
{
public:
    explicit UserInfoPanel(IChannelController &controller);

    void Draw() override;

    void Update() override;

    bool IsVisible() override;

private:
    IChannelController &controller;
};