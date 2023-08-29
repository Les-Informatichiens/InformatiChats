//
// Created by Jean on 8/17/2023.
//

#pragma once

#include "Model/Data-Access/IChatAPI.h"
#include "Model/Models/User.h"

class Model
{
public:
    void LoginWithNewUser(const std::string& username_);
    void AddNewChatPeer(const std::string& peerId);

    std::unordered_map<std::string, PeerData> GetPeerDataMap();

    std::string GetOwnUsername() const;
    int GetMaxNameLength() const { return maxNameLength; };
    bool IsClientConnected() const { return this->chatClient.IsConnected(); };

    void SetSelectedPeerId(const std::string& peerId);

private:
    void SetUser(IUser& user_);
    void SetChatClient(IChatAPI& chatClient);

private:
    const int maxNameLength{32};

private:
    IUser& user;
    IChatAPI& chatClient;
};
