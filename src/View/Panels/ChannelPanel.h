#pragma once

#include "IPanel.h"
#include "imgui.h"
#include <Controller/IChannelController.h>
#include <Model/EventBus.h>
#include <misc/cpp/imgui_stdlib.h>
#include <nlohmann/json.hpp>

class ChannelPanel : public IPanel
{
public:
    explicit ChannelPanel(IChannelController& channelController, EventBus& eventBus);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

    bool addNewChatPrompt{false};

    std::string selectedChat;
    IChannelController& controller;
    EventBus& eventBus;

    std::string usernameToConnectToBuf;
    std::string fingerprintToQueryBuf;
    nlohmann::json userFingerprintResponse;
};
