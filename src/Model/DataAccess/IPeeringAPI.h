//
// Created by Jonathan Richard on 2023-09-17.
//

#pragma once

#include "Model/Models/PeerData.h"
#include <functional>
#include <string>

struct PeerConnectionStateChangeEvent
{
    std::string peerId;
    ConnectionState connectionState;
};

class IPeeringAPI
{
public:
    virtual void AttemptToConnectToPeer(const std::string& peerId) = 0;
//    virtual void
    virtual void OnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback) = 0;
    virtual void OnPeerRequest(std::function<bool (std::string)> callback) = 0;
};
