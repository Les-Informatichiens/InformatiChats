//
// Created by Jean on 8/29/2023.
//
#pragma once

#include "Model/Models/User.h"
#include "UserLogic.h"

class UserLogic
{
public:
    explicit UserLogic(User& user) : user(user){};

    void AddChatMessageToSelectedChatHistory(ChatMessage chatMessage);
    void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage);
    void CreateNewChatHistory(const std::string& peerId);
    void UpdatePeerState(const std::string& peerId, const ConnectionState& state);
    void IncrementPeerUnreadMessageCount(const std::string& peerId);
    void Reset(const std::string& username);
    [[nodiscard]] const std::string& GetUserName() const;
    const void SendTextMessage(const std::string& message);

private:
    User& user;
};
