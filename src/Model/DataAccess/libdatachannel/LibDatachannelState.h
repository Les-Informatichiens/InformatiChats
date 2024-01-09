//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "LibDatachannelPeer.h"
#include <unordered_map>
#include <utility>

class LibDatachannelState
{
public:
    virtual ~LibDatachannelState();

    void Reset();

    void RegisterPeer(const std::shared_ptr<LibDatachannelPeer>& peer);
    void DestroyPeer(const std::string& peerId);
    std::shared_ptr<LibDatachannelPeer> GetPeer(const std::string& peerId);

public:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<LibDatachannelPeer>> peerMap;
};
