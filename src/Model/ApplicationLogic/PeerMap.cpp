//
// Created by Jonathan Richard on 2024-01-04.
//

#include "PeerMap.h"


void PeerMap::AddPeer(const std::shared_ptr<Peer>& peer)
{
    this->peerMap.emplace(peer->GetFingerprint(), peer);
}

void PeerMap::RemovePeer(const std::string& fingerprint)
{
    this->peerMap.erase(fingerprint);
}

std::shared_ptr<Peer> PeerMap::GetPeer(const std::string& fingerprint)
{
    const auto peerIt = this->peerMap.find(fingerprint);
    if (peerIt != this->peerMap.end())
    {
        return peerIt->second;
    }
    return nullptr;
}

std::shared_ptr<Peer> PeerMap::TryGetPeer(const std::string& fingerprint)
{
    const auto peerIt = this->peerMap.find(fingerprint);
    if (peerIt != this->peerMap.end())
    {
        return peerIt->second;
    }
    return nullptr;
}

std::shared_ptr<Peer> PeerMap::TryGetPeerBySignalingId(const std::string& signalingId)
{
    for (const auto& peer: this->peerMap | std::views::values)
    {
        if (peer->GetSignalingId() == signalingId)
        {
            return peer;
        }
    }
    return nullptr;
}

bool PeerMap::ContainsPeer(const std::string& fingerprint) const
{
    return this->peerMap.contains(fingerprint);
}

bool PeerMap::ContainsPeerBySignalingId(const std::string& signalingId) const
{
    for (const auto& peer: this->peerMap | std::views::values)
    {
        if (peer->GetSignalingId() == signalingId)
        {
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<Peer>> PeerMap::GetPeers() const
{
    auto view = this->peerMap | std::views::values;
    std::vector peers(view.begin(), view.end());
    return peers;
}

std::vector<std::shared_ptr<Peer>> PeerMap::GetUnprocessedPeers() const
{
    auto view = this->unprocessedPeers | std::views::values;
    std::vector peers(view.begin(), view.end());
    return peers;
}

void PeerMap::AddUnprocessedPeer(const std::string& tempUUID, const std::shared_ptr<Peer>& peer)
{
    this->unprocessedPeers.emplace(tempUUID, peer);
}

std::shared_ptr<Peer> PeerMap::AcceptUnprocessedPeer(const std::string& tempUUID)
{
    const auto peerIt = this->unprocessedPeers.find(tempUUID);
    if (peerIt != this->unprocessedPeers.end())
    {
        // this->peerMap.insert_or_assign(peerIt->second->GetFingerprint(), peerIt->second);
        auto peer = peerIt->second;
        this->unprocessedPeers.erase(peerIt);
        return peer;
    }
    return nullptr;
}

void PeerMap::RejectUnprocessedPeer(const std::string& tempUUID)
{
    this->unprocessedPeers.erase(tempUUID);
}
