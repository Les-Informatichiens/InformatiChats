//
// Created by Jonathan Richard on 2024-01-04.
//

#pragma once

#include <Model/Models/Peer.h>
#include <string>
#include <unordered_map>


class PeerMap
{
public:
    PeerMap() = default;

    void AddPeer(const std::shared_ptr<Peer>& peer);
    void RemovePeer(const std::string& fingerprint);
    std::shared_ptr<Peer> GetPeer(const std::string& fingerprint);
    std::shared_ptr<Peer> TryGetPeer(const std::string& fingerprint);
    std::shared_ptr<Peer> TryGetPeerBySignalingId(const std::string& signalingId);

    [[nodiscard]] bool ContainsPeer(const std::string& fingerprint) const;
    [[nodiscard]] bool ContainsPeerBySignalingId(const std::string& signalingId) const;

    [[nodiscard]] std::vector<std::shared_ptr<Peer>> GetPeers() const;
    [[nodiscard]] std::vector<std::shared_ptr<Peer>> GetUnprocessedPeers() const;

    void AddUnprocessedPeer(const std::string& tempUUID, const std::shared_ptr<Peer>& peer);
    std::shared_ptr<Peer> AcceptUnprocessedPeer(const std::string& tempUUID);
    void RejectUnprocessedPeer(const std::string& tempUUID);

private:
    std::unordered_map<std::string, std::shared_ptr<Peer>> peerMap;
    std::unordered_map<std::string, std::shared_ptr<Peer>> unprocessedPeers;
};
