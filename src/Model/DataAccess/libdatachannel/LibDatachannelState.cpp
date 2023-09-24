//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelState.h"
#include "nlohmann/json.hpp"


void LibDatachannelState::Reset()
{
    this->peerMap.clear();
}

LibDatachannelState::~LibDatachannelState()
{
    this->Reset();
}

void LibDatachannelState::RegisterPeer(const std::shared_ptr<Peer>& peer)
{
    this->peerMap.insert_or_assign(peer->GetId(), peer);
}

void LibDatachannelState::DestroyPeer(const std::string& peerId)
{
    auto pcIt = this->peerMap.find(peerId);
    if (pcIt == this->peerMap.end())
    {
        return;
    }
    pcIt->second.reset();
    this->peerMap.erase(pcIt);
}

std::shared_ptr<Peer> LibDatachannelState::GetPeer(const std::string& peerId)
{
    auto pcIt = this->peerMap.find(peerId);
    if (pcIt == this->peerMap.end())
    {
        return nullptr;
    }
    return pcIt->second;
}
