#pragma once

#include "ChatMessage.h"
#include <Model/Models/PeerData.h>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>


using ChatHistory = std::vector<ChatMessage>;

class User
{
private:
    explicit User(std::string username) : username(std::move(username)){};

    std::string username;
    static constexpr size_t maxNameLength{32};

    std::unordered_map<std::string, PeerData> peerDataMap;
    std::string selectedChat;

    // TODO: better management for chat histories
    // peerId, History for that peer id
    std::unordered_map<std::string, ChatHistory> chatHistories;

    friend class UserLogic;

public:
    User() = default;
};
