//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelState.h"
#include "nlohmann/json.hpp"


void LibDatachannelState::Reset()
{
    this->peerMap.clear();
}

// LibDatachannelState::~LibDatachannelState()
// {
//     this->Reset();
// }

void LibDatachannelState::RegisterPeer(const std::shared_ptr<LibDatachannelPeer>& peer)
{
    this->peerMap.insert_or_assign(peer->GetId(), peer);
    this->Cleanup();
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

std::shared_ptr<LibDatachannelPeer> LibDatachannelState::GetPeer(const std::string& peerId)
{
    auto pcIt = this->peerMap.find(peerId);
    if (pcIt == this->peerMap.end())
    {
        return nullptr;
    }
    return pcIt->second;
}

std::shared_ptr<LibDatachannelPeer> LibDatachannelState::GetPeerBySignalingId(const std::string& signalingId)
{
    const auto pcIt = std::ranges::find_if(this->peerMap, [&signalingId](const auto& pair) {
        return pair.second->GetSignalingId() == signalingId;
    });
    if (pcIt == this->peerMap.end())
    {
        return nullptr;
    }
    return pcIt->second;
}

void LibDatachannelState::Cleanup()
{
    for (auto it = this->peerMap.begin(); it != this->peerMap.end();)
    {
        if (it->second->State() == rtc::PeerConnection::State::Closed || it->second->State() == rtc::PeerConnection::State::Failed || it->second->State() == rtc::PeerConnection::State::Disconnected)
        {
            it = this->peerMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
