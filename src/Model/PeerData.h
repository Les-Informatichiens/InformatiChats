//
// Created by Jonathan Richard on 2023-08-19.
//

#pragma once

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
    size_t unreadMessageCount;
    ConnectionState connectionState;
};
