#include "User.h"

#include <utility>


User::User(std::string username_)
: username(std::move(username_))
{
}

std::string User::GetUsername() const
{
    return this->username;
}

const std::unordered_map<std::string, PeerData>& User::GetPeerDataMap() const
{
    return this->peerDataMap;
}

void User::UpdatePeerState(const std::string& peerId, ConnectionState state)
{
    auto result = this->peerDataMap.insert({peerId, {}});
    result.first->second.connectionState = state;
}

void User::IncrementPeerUnreadMessageCount(const std::string& peerId)
{
    auto result = this->peerDataMap.insert({peerId, {}});
    ++result.first->second.unreadMessageCount;
}
void User::Reset(std::string username_)
{
    *this = User(std::move(username_));
}
