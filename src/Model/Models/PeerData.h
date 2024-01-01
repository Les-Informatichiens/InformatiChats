//
// Created by Jonathan Richard on 2023-08-19.
//

#pragma once

#include <cstddef>
#include <Model/Models/UserData.h>


enum ConnectionState
{
    New,
    Connecting,
    Connected,
    Disconnected,
    Failed,
    Closed
};

struct PeerData
{
    UserProfile profile;
    size_t unreadMessageCount = 0;
    ConnectionState connectionState{};
    bool authenticated = false;
};
