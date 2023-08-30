#pragma once

#include "IPanel.h"
#include "imgui.h"
#include <Controller/IChannelController.h>
#include <misc/cpp/imgui_stdlib.h>

class ChannelPanel : public IPanel
{
public:
    explicit ChannelPanel(IChannelController& channelController);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

    bool addNewChatPrompt{false};

    std::string selectedChat;
    IChannelController& controller;

    std::string usernameToConnectToBuf;
};
