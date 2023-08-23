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
    if (state == ConnectionState::Failed || state == ConnectionState::Closed || state == ConnectionState::Disconnected)
    {
        this->peerDataMap.erase(result.first->first);
        return;
    }
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

void User::SetSelectedPeerId(std::string selectedChat_)
{
    this->selectedChat = std::move(selectedChat_);
}

const std::string& User::GetSelectedPeerId() const
{
    return this->selectedChat;
}

const ChatHistory* User::GetSelectedChatHistory() const
{
    const auto chatHistoryIt = this->chatHistories.find(this->selectedChat);
    if (chatHistoryIt != this->chatHistories.end())
    {
        return &chatHistoryIt->second;
    }

    return nullptr;
}

ChatHistory* User::GetSelectedChatHistory()
{
    auto chatHistoryIt = this->chatHistories.find(this->selectedChat);
    if (chatHistoryIt != this->chatHistories.end())
    {
        return &chatHistoryIt->second;
    }

    return nullptr;
}

void User::AddChatMessageToSelectedChatHistory(ChatMessage chatMessage_)
{
    if (ChatHistory* chatHistory = this->GetSelectedChatHistory())
    {
        chatHistory->push_back(std::move(chatMessage_));
    }
}

void User::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage)
{
    const auto chatHistoryIt = this->chatHistories.find(peerId);
    if (chatHistoryIt != this->chatHistories.end())
    {
        chatHistoryIt->second.push_back(chatMessage);
    }
}

void User::CreateNewChatHistory(const std::string& peerId_)
{
    this->chatHistories.insert({peerId_, ChatHistory{}});
}
