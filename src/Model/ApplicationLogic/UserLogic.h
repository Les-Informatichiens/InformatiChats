//
// Created by Jean on 8/29/2023.
//
#pragma once

#include "Model/DataAccess/IChatAPI.h"
#include "Model/Models/User.h"

class UserLogic
{
public:
    UserLogic(User& user, IChatAPI& chatAPI) : user(user), chatAPI(chatAPI){};

    void AppendSelectedChatHistory(const std::string& message);
    void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage);
    void CreateNewChatHistory(const std::string& peerId);
    void UpdatePeerState(const std::string& peerId, const ConnectionState& state);
    void IncrementPeerUnreadMessageCount(const std::string& peerId);
    void Reset(const std::string& username);
    [[nodiscard]] const std::string& GetUserName() const;
    [[nodiscard]] const size_t& GetMaxNameLength() const;
    void LoginWithNewUser(const std::string& username_);
    bool IsClientConnected() const;
    [[nodiscard]] const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const;
    const void SendTextMessage(const std::string& message);
    void AddNewChatPeer(const std::string& peerId);
    void SetSelectedPeerId(const std::string& peerId);
    [[nodiscard]] const ChatHistory* GetSelectedChatHistory() const;

private:
    User& user;
    IChatAPI& chatAPI;
};
