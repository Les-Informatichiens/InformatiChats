#pragma once

#include "IPanel.h"
#include <Controller/IChannelController.h>
#include <Model/Chat.h>

#include "imgui.h"
#include <unordered_map>


class ChannelPanel : public IPanel
{
public:
    explicit ChannelPanel(IChannelController& channelController);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

    bool addNewChatPrompt{false};

    std::string selectedChat;
    ExampleAppConsole console;
    IChannelController& controller;

    std::string UsernameToConnectToBuf;
};
