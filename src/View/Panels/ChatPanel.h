#pragma once

#include "IPanel.h"
#include <Controller/IChatController.h>
#include <Model/Chat.h>

#include "../console.h"


class ChatPanel : public IPanel
{
public:
    explicit ChatPanel(IChatController &channelController);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

private:
    ExampleAppConsole console{};

    bool consoleOpen{true};
    std::string selectedChat;

    struct PeerData
    {
        ImVector<char *> history;
        size_t unreadMessageCount;
    };
    // chat histories
    std::unordered_map<std::string, PeerData> historyMap;
    IChatController &controller;

    char inputBuf[256]{};
};
