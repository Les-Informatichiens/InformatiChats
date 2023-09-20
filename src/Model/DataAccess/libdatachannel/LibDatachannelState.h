//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "rtc/datachannel.hpp"
#include "rtc/peerconnection.hpp"

#include <unordered_map>
#include <utility>
#include "rtc/websocket.hpp"

struct Peer
{
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
};

class LibDatachannelState
{
public:
    void Reset();

    virtual ~LibDatachannelState();
    void SetPeerChannel(const std::string& peerId, const std::shared_ptr<rtc::DataChannel>& dc);
    void RegisterPeerConnection(const std::string& peerId, const std::shared_ptr<rtc::PeerConnection>& pc);
    void ClosePeerConnection(const std::string& peerId);
    std::shared_ptr<rtc::PeerConnection> GetPeerConnection(const std::string& peerId);
    Peer GetPeer(const std::string& peerId);

public:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, Peer> peerMap;
};
