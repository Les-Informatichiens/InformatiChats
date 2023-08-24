//
// Created by Jonathan Richard on 2023-08-19.
//

#pragma once

#include "PeerData.h"

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>

struct ChatMessage
{
    std::string content;
    std::chrono::milliseconds timestamp;
    std::string senderId;
};

using ChatHistory = std::vector<ChatMessage>;


class IUser
{
public:
    virtual void Reset(std::string username) = 0;

    virtual std::string GetUsername() const = 0;
    virtual const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const = 0;

    virtual void SetSelectedPeerId(std::string selectedChat) = 0;
    virtual const std::string& GetSelectedPeerId() const = 0;
    virtual const ChatHistory* GetSelectedChatHistory() const = 0;

    virtual void CreateNewChatHistory(const std::string& peerId) = 0;
    virtual void AddChatMessageToSelectedChatHistory(ChatMessage chatMessage) = 0;
    virtual void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage) = 0;

    virtual void UpdatePeerState(const std::string& peerId, ConnectionState state) = 0;
    virtual void IncrementPeerUnreadMessageCount(const std::string& peerId) = 0;
};
