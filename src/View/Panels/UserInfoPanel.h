//
// Created by Jean on 8/17/2023.
//

#pragma once

#include "IPanel.h"
#include <Controller/IChannelController.h>
#include <Model/Chat.h>


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
