//
// Created by Jonathan Richard on 2023-09-17.
//

#pragma once

#include "Model/DataAccess/libdatachannel/Peer.h"
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

    virtual void OpenPeerConnection(const std::string& peerId, std::function<void()> onReady) = 0;
    virtual void ClosePeerConnection(const std::string& peerId) = 0;

    virtual void SendMessage(const std::string& peerId, const BaseMessage<MessageType>& message) = 0;

    virtual void OnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback) = 0;
    virtual void OnPeerRequest(std::function<bool(const std::string&)> callback) = 0;
    virtual void OnNewPeer(std::function<void(const std::string&)> callback) = 0;
    virtual void OnPeerMessage(const std::string& peerId, std::function<void(BaseMessage<MessageType>&)> callback) = 0;
    virtual void OnPeerConnected(const std::string& peerId, std::function<void()> callback) = 0;
};