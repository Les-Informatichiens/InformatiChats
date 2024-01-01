//
// Created by Jonathan Richard on 2023-10-02.
//

#pragma once

#include <Model/Models/Peer.h>

class PeerManager
{
    explicit PeerManager();
    std::unordered_map<std::string, Peer> peerMap;
};
