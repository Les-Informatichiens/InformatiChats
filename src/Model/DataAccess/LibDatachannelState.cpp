//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelState.h"
#include "nlohmann/json.hpp"

#include <utility>

void LibDatachannelState::Reset()
{
    this->peerConnectionMap.clear();
}

void LibDatachannelState::RegisterPeerConnection(const std::string& peerId, std::shared_ptr<rtc::PeerConnection> pc)
{
    this->peerConnectionMap.emplace(peerId, pc);
}

void LibDatachannelState::ClosePeerConnection(const std::string& peerId)
{
    auto pcIt = this->peerConnectionMap.find(peerId);
    if (pcIt == this->peerConnectionMap.end())
    {
        return;
    }
    pcIt->second.reset();
    peerConnectionMap.erase(pcIt);
}

std::shared_ptr<rtc::PeerConnection> LibDatachannelState::GetPeerConnection(const std::string& peerId)
{
    auto pcIt = this->peerConnectionMap.find(peerId);
    if (pcIt == this->peerConnectionMap.end())
    {
        return nullptr;
    }
    return pcIt->second;
}
