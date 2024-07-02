//
// Created by Jonathan Richard on 2024-01-03.
//

#pragma once

#include "Model/Models/ConnectionState.h"
#include "Model/Models/LocalUserData.h"


#include <string>

struct RemoteUserDTO
{
    std::string peerId;
    std::string fingerprint;
    std::string username;
    UserProfile profile;
    size_t unreadMessageCount = 0;
    ConnectionState connectionState{};
    bool authenticated = false;
};
