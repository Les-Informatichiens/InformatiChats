//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "IConnectionAPI.h"
#include "LibDatachannelState.h"

#include <rtc/peerconnection.hpp>
#include <rtc/websocket.hpp>

#include <future>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

class LibDatachannelConnectionAPI : public IConnectionAPI
{
public:
    explicit LibDatachannelConnectionAPI(LibDatachannelState& state);

    void ConnectWithUsername(const std::string& username) override;
    void AttemptToConnectToPeer(const std::string& peerId) override;


private:
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string& peerId);

private:
    LibDatachannelState& state;

    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::shared_ptr<rtc::WebSocket> webSocket;
    std::promise<void> wsPromise;
    bool connected{};
    std::string username;
};
