//
// Created by Jonathan Richard on 2023-08-19.
//

#pragma once


class IUser
{
public:
    virtual void Reset(std::string username) = 0;

    virtual std::string GetUsername() const = 0;
    virtual const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const = 0;

    virtual void UpdatePeerState(const std::string& peerId, ConnectionState state) = 0;
    virtual void IncrementPeerUnreadMessageCount(const std::string& peerId) = 0;
};
