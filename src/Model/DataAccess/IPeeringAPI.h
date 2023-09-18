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

struct PeeringConfig
{
    std::string stunServer;
    std::string stunServerPort;
};

class IPeeringAPI
{
public:
    virtual void Init(const PeeringConfig& peeringConfig) = 0;
    
    virtual void AttemptToConnectToPeer(const std::string& peerId) = 0;

    virtual void OnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback) = 0;
    virtual void OnPeerRequest(std::function<bool(std::string)> callback) = 0;
};
