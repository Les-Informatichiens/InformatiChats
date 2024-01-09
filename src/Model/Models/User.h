#pragma once

#include "ChatMessageInfo.h"
#include "Peer.h"
#include <Model/Models/PeerData.h>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <Model/Models/UserData.h>


/**
 * \brief Represents the active user in the application
 */
class User
{
private:
    explicit User(std::string username) : username(std::move(username)){};
    explicit User(UserData userData) : userData(std::move(userData)){};

    std::string username;
    static constexpr size_t maxNameLength{32};

    std::unordered_map<std::string, Peer> peerMap;
    std::string selectedChat;

    UserData userData;

    std::string decryptedPrivateKey;

    friend class UserLogic;
    friend class UserDataManager;
public:
    User() = default;
};
