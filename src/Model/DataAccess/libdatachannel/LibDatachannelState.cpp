//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelState.h"
#include "nlohmann/json.hpp"

#include <utility>

void LibDatachannelState::Reset()
{
    this->peerConnectionMap.clear();
    this->peerMap.clear();
}

void LibDatachannelState::SetPeerChannel(const std::string& peerId, const std::shared_ptr<rtc::DataChannel>& dc)
{
    auto pcIt = this->peerMap.find(peerId);
    if (pcIt == this->peerMap.end())
    {
        return;
    }
    pcIt->second.dc = dc;
}

void LibDatachannelState::RegisterPeerConnection(const std::string& peerId, const std::shared_ptr<rtc::PeerConnection>& pc)
{
    this->peerConnectionMap.insert_or_assign(peerId, pc);
    this->peerMap.insert_or_assign(peerId, Peer{pc, nullptr});
}

LibDatachannelState::~LibDatachannelState()
{
    this->Reset();
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
    auto pcIt = this->peerMap.find(peerId);
    if (pcIt == this->peerMap.end())
    {
        return nullptr;
    }
    return pcIt->second.pc;
}

Peer LibDatachannelState::GetPeer(const std::string& peerId)
{
    auto pcIt = this->peerMap.find(peerId);
    if (pcIt == this->peerMap.end())
    {
        return {};
    }
    return pcIt->second;
}
