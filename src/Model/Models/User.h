#pragma once

#include "Model/Models/User.h"
#include "PeerData.h"
#include <chrono>
#include <string>
#include <unordered_map>

struct ChatMessage
{
    std::string content;
    std::chrono::milliseconds timestamp;
    std::string senderId;
};

using ChatHistory = std::vector<ChatMessage>;


class User
{
public:
    User() = default;
    explicit User(std::string username);

    //void SetUsername(std::string newUsername);
    [[nodiscard]] const std::string& GetUsername() const;
    [[nodiscard]] const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const;

    void SetSelectedPeerId(std::string selectedChat);
    [[nodiscard]] const std::string& GetSelectedPeerId() const;
    [[nodiscard]] const ChatHistory& GetSelectedChatHistory() const;
    [[nodiscard]] const ChatHistory& GetChatHistory(const std::string& peerId) const;
    [[nodiscard]] const std::unordered_map<std::string, ChatHistory>& GetChatHistories() const;

private:
    std::string username;

    std::unordered_map<std::string, PeerData> peerDataMap;

    std::string selectedChat;

    // TODO: better management for chat histories
    // peerId, History for that peer id
    std::unordered_map<std::string, ChatHistory> chatHistories;
};
