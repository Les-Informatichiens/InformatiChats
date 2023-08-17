#pragma once

#include "IPanel.h"
#include "../console.h"
#include "../../Model/Chat.h"

class ChatPanel: public IPanel {
public:
    explicit ChatPanel(Chat &chatClient);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

    ExampleAppConsole console{};
    Chat& chatClient;

    bool consoleOpen{true};
    std::string selectedChat;

    struct PeerData
    {
        ImVector<char*> history;
        size_t unreadMessageCount;
    };
    // chat histories
    std::unordered_map<std::string, PeerData> historyMap;

};
