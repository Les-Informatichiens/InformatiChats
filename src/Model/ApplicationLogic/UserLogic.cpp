//
// Created by Jean on 8/29/2023.
//

#include "UserLogic.h"


const ChatHistory* UserLogic::GetSelectedChatHistory() const
{
    if (!user.selectedChat.empty() && !user.chatHistories.empty())
    {
        return &user.chatHistories.at(user.selectedChat);
    }
    return nullptr;
}

void UserLogic::AppendSelectedChatHistory(const std::string& message)
{
    auto chatHistory = user.chatHistories.find(user.selectedChat);

    if (chatHistory != user.chatHistories.end())
        chatHistory->second.emplace_back(message, duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()), user.username);
}

void UserLogic::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage)
{
    auto chatHistory = user.chatHistories.find(peerId);
    if (chatHistory != user.chatHistories.end())
        chatHistory->second.emplace_back(chatMessage);
}

void UserLogic::CreateNewChatHistory(const std::string& peerId_)
{
    auto& chatHistories = user.chatHistories;
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
    ++user.peerDataMap.at(peerId).unreadMessageCount;
}

void UserLogic::Reset(const std::string& username)
{
    user = User{username};
}

const std::string& UserLogic::GetUserName() const
{
    return user.username;
}

const void UserLogic::SendTextMessage(const std::string& message)
{
    chatAPI.SendMessageToPeer(user.selectedChat, message);
}

const std::unordered_map<std::string, PeerData>& UserLogic::GetPeerDataMap() const
{
    return user.peerDataMap;
}

void UserLogic::AddNewChatPeer(const std::string& peerId)
{
    chatAPI.AttemptToConnectToPeer(peerId);
    CreateNewChatHistory(peerId);
}

void UserLogic::SetSelectedPeerId(const std::string& peerId)
{
    user.selectedChat = peerId;
}

const size_t& UserLogic::GetMaxNameLength() const
{
    return user.maxNameLength;
}

void UserLogic::LoginWithNewUser(const std::string& username_)
{
    Reset(username_);
    chatAPI.Reset();
    //this->SetUser(user_);

    chatAPI.SetOnPeerConnectionStateChange([this](const PeerConnectionStateChangeEvent& e) {
        UpdatePeerState(e.peerId, e.connectionState);
        if (e.connectionState == ConnectionState::Connected)
        {
            CreateNewChatHistory(e.peerId);
        }
    });
    chatAPI.SetOnMessageReceived([this](const ChatMessage& e) {
        IncrementPeerUnreadMessageCount(e.senderId);
        AddChatMessageToPeerChatHistory(e.senderId,
                                        {e.content,
                                         duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                                         e.senderId});
    });

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "informatichiens.com";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = {stunServer, stunServerPort, signalingServer, signalingServerPort};
    chatAPI.Init(config);

    chatAPI.AttemptConnectionWithUsername(username_);
}

bool UserLogic::IsClientConnected() const
{
    return chatAPI.IsConnected();
}
