#pragma once

#include "IPanel.h"
#include <Model/Chat.h>

#include "imgui.h"

#include <unordered_map>

class ChannelPanel: public IPanel {
public:

    explicit ChannelPanel(Chat &chatClient);

    void Draw() override;
    void Update() override;
    bool IsVisible() override;

    bool addNewChatPrompt{false};

    struct PeerData
    {
        ImVector<char*> history;
        size_t unreadMessageCount;
    };
    std::unordered_map<std::string, PeerData> historyMap;
    std::string selectedChat;
    Chat& chatClient;
    ExampleAppConsole console;
    static constexpr int maxNameLength{32};


};
