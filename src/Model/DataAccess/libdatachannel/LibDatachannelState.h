//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "rtc/datachannel.hpp"
#include "rtc/peerconnection.hpp"

#include <unordered_map>
#include <utility>
#include "rtc/websocket.hpp"

class LibDatachannelState
{
public:
    void Reset();

    void RegisterPeerConnection(const std::string& peerId, const std::shared_ptr<rtc::PeerConnection>& pc);
    void ClosePeerConnection(const std::string& peerId);
    std::shared_ptr<rtc::PeerConnection> GetPeerConnection(const std::string& peerId);

public:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
};
