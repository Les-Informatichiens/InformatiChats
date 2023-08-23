#pragma once

#include "IPanel.h"
#include <Controller/IChatController.h>


class ChatPanel : public IPanel
{
public:
    explicit ChatPanel(IChatController& channelController);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    std::unordered_map<std::string, ChatMessage> lastChatMessages;

    bool consoleOpen{true};
    std::string selectedChat;

    IChatController& controller;

    std::string inputBuf;
};
