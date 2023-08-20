//
// Created by Jean on 8/17/2023.
//

#pragma once

#include "ChatClient.h"
#include "User.h"


class Model
{
public:
    Model();

    void LoginWithNewUser(const std::string& username_);
    void AddNewChatPeer(const std::string& peerId);

    std::unordered_map<std::string, PeerData> GetPeerDataMap();

    std::string GetOwnUsername() const;
    int GetMaxNameLength() const { return maxNameLength; };
    bool IsClientConnected() const { return this->chatClient != nullptr && this->chatClient->IsConnected(); };

private:
    void SetUser(std::unique_ptr<User> user_);
    void SetChatClient(std::unique_ptr<ChatClient> chatClient);

private:
    const int maxNameLength{32};

private:
    std::unique_ptr<User> user;
    std::unique_ptr<ChatClient> chatClient;
};
