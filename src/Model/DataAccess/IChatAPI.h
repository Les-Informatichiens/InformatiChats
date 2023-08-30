//
// Created by Jonathan Richard on 2023-08-19.
//

#pragma once

#include "Model/Models/PeerData.h"

#include <functional>
#include <string>

struct MessageReceivedEvent
{
    std::string senderId;
    std::string content;
};

struct PeerConnectionStateChangeEvent
{
    std::string peerId;
    ConnectionState connectionState;
};

struct ConnectionConfig
{
    std::string stunServer;
    std::string stunServerPort;
    std::string signalingServer;
    std::string signalingServerPort;
};


class IChatAPI
{
public:
    virtual void Reset() = 0;

    virtual void Init(const ConnectionConfig& config) = 0;

    virtual bool ICEServerExists() const = 0;
    virtual bool IsConnected() const = 0;

    virtual void AttemptConnectionWithUsername(const std::string& newUsername) = 0;
    virtual void AttemptToConnectToPeer(const std::string& peerId) = 0;

    virtual void SetOnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback) = 0;
    virtual void SetOnMessageReceived(std::function<void(MessageReceivedEvent)> callback) = 0;

    virtual const void SendMessageToPeer(const std::string& peerId, const std::string& message) = 0;
};
