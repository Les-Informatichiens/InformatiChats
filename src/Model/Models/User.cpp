#include "User.h"

#include <utility>


User::User(std::string username_)
    : username(std::move(username_))
{
}

const std::string& User::GetUsername() const
{
    return this->username;
}

const std::unordered_map<std::string, PeerData>& User::GetPeerDataMap() const
{
    return this->peerDataMap;
}

void User::SetSelectedPeerId(std::string selectedChat_)
{
    this->selectedChat = std::move(selectedChat_);
}

const std::string& User::GetSelectedPeerId() const
{
    return this->selectedChat;
}

const ChatHistory& User::GetSelectedChatHistory() const
{
    return this->chatHistories.find(this->selectedChat)->second;
}

const ChatHistory& User::GetChatHistory(const std::string& peerId) const
{
    return this->chatHistories.find(peerId)->second;
}
const std::unordered_map<std::string, ChatHistory>& User::GetChatHistories() const
{
    return chatHistories;
}
