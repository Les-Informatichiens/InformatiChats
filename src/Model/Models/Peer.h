//
// Created by Jonathan Richard on 2023-09-30.
//

#pragma once

#include "ChatMessageInfo.h"
#include "Exchange.h"
#include "PeerData.h"


#include <vector>

using ChatHistory = std::vector<ChatMessageInfo>;

class Peer
{
public:
    Peer(const std::string& username)
        : username(username)
    {
    }
private:
    std::string username;
    std::string verifiedPublicKey;
    std::string ipAddress;

    PeerData peerData{};
    ChatHistory chatHistory;

    ExchangeManager ongoingExchanges;

    friend class User;
    friend class UserLogic;
    friend class UserDataManager;
};
