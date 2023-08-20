#pragma once

#include "ChatClient.h"
#include "PeerData.h"
#include "IUser.h"


class User
{
public:
    explicit User(std::string username);

    void Reset(std::string username) override;

    //void SetUsername(std::string newUsername);
    [[nodiscard]] std::string GetUsername() const;
    [[nodiscard]] const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const;

    void UpdatePeerState(const std::string& peerId, ConnectionState state);
    void IncrementPeerUnreadMessageCount(const std::string& peerId);

private:
    std::string username;

    std::unordered_map<std::string, PeerData> peerDataMap;
};
