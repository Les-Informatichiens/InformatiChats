#pragma once

#include "ChatMessageInfo.h"
#include "Model/ApplicationLogic/ChannelMap.h"
#include "Model/ApplicationLogic/PeerMap.h"
#include "Model/ApplicationLogic/RemoteUserMap.h"
#include "Peer.h"

#include <Model/Models/LocalUserData.h>
#include <Model/Models/PeerData.h>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>


/**
 * \brief Represents the active user in the application
 */
class LocalUser
{
private:
    explicit LocalUser(std::string username) : username(std::move(username)){};
    explicit LocalUser(LocalUserData userData) : userData(std::move(userData)){};

    std::string username;
    static constexpr size_t maxNameLength{32};

    RemoteUserMap remoteUserMap{};
    PeerMap peerMap_{};
    ChannelMap channelMap{};
    // std::unordered_map<std::string, Peer> peerMap;
    std::string selectedChat;

    LocalUserData userData;

    std::string decryptedPrivateKey;

    friend class UserLogic;
    friend class UserDataManager;
public:
    LocalUser() = default;
};
