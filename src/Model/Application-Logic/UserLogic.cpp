//
// Created by Jean on 8/29/2023.
//

#include "UserLogic.h"
#include "Model/Data-Access/IChatAPI.h"
#include "Model/Models/User.h"
void UserLogic::AddChatMessageToSelectedChatHistory(ChatMessage chatMessage_)
{
    auto chatHistory = user.GetSelectedChatHistory();
    chatHistory.push_back(std::move(chatMessage_));
}

void UserLogic::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage)
{
    auto chatHistoryIt = user.GetChatHistory(peerId);
    chatHistoryIt.emplace_back(chatMessage);
}

void UserLogic::CreateNewChatHistory(const std::string& peerId_)
{
    auto chatHistories = user.GetChatHistories();
    chatHistories.emplace(peerId_, ChatHistory{});
}

void UserLogic::UpdatePeerState(const std::string& peerId, const ConnectionState& state)
{
    auto result = user.peerDataMap.insert({peerId, {}});
    if (state == ConnectionState::Failed || state == ConnectionState::Closed || state == ConnectionState::Disconnected)
    {
        user.peerDataMap.erase(result.first->first);
        return;
    }
    result.first->second.connectionState = state;
}

void UserLogic::IncrementPeerUnreadMessageCount(const std::string& peerId)
{
    auto result = user.peerDataMap.insert({peerId, {}});
    ++result.first->second.unreadMessageCount;
}

void UserLogic::Reset(const std::string& username)
{
    user = User(username);
}
const std::string& UserLogic::GetUserName() const
{
    return user.GetUsername();
}
const void UserLogic::SendTextMessage(const std::string& message)
{
}
