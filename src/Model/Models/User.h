#pragma once

#include "ChatMessageInfo.h"
#include "Peer.h"
#include <Model/Models/PeerData.h>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>



class User
{
private:
    explicit User(std::string username) : username(std::move(username)){};

    std::string username;
    static constexpr size_t maxNameLength{32};

    std::unordered_map<std::string, Peer> peerMap;
    std::string selectedChat;

    friend class UserLogic;

public:
    User() = default;
};
