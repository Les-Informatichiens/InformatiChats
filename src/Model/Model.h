//
// Created by Jean on 8/17/2023.
//

#pragma once

#include "ChatClient.h"
#include "IChatClient.h"
#include "IUser.h"
#include "User.h"


class Model
{
public:
    Model();
    Model(IUser& user, IChatClient& chatClient);

    void LoginWithNewUser(const std::string& username_);
    void AddNewChatPeer(const std::string& peerId);

    std::unordered_map<std::string, PeerData> GetPeerDataMap();

    std::string GetOwnUsername() const;
    int GetMaxNameLength() const { return maxNameLength; };
    bool IsClientConnected() const { return this->chatClient.IsConnected(); };

    void SendMessage(const std::string& peerId, const std::string& message) const;

    void SetSelectedPeerId(const std::string& peerId);
    const std::string& GetSelectedPeerId() const;
    const ChatHistory* GetSelectedChatHistory();

private:
    void SetUser(IUser& user_);
    void SetChatClient(IChatClient& chatClient);

private:
    const int maxNameLength{32};

private:
    IUser& user;
    IChatClient& chatClient;
};
