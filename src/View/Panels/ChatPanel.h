#pragma once

#include "IPanel.h"
#include "imgui.h"
#include <Controller/IChatController.h>
#include <iomanip>
#include <misc/cpp/imgui_stdlib.h>
#include <util/string_util.h>

class ChatPanel : public IPanel
{
public:
    explicit ChatPanel(IChatController& channelController);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    // std::unordered_map<std::string, ChatMessage> lastChatMessages;

    bool consoleOpen{true};
    std::string selectedChat;

    IChatController& controller;

    std::string inputBuf;
};
