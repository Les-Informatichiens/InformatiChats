//
// Created by Jean on 8/29/2023.
//

#include "UserLogic.h"


const ChatHistory* UserLogic::GetSelectedChatHistory() const
{
    if (!this->user.selectedChat.empty() && !this->user.chatHistories.empty())
    {
        return &this->user.chatHistories.at(this->user.selectedChat);
    }
    return nullptr;
}

void UserLogic::AppendSelectedChatHistory(const std::string& message)
{
    auto chatHistory = this->user.chatHistories.find(this->user.selectedChat);

    if (chatHistory != this->user.chatHistories.end())
    {
        chatHistory->second.emplace_back(
                message,
                duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                this->user.username);
    }
}

void UserLogic::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage)
{
    auto chatHistory = this->user.chatHistories.find(peerId);
    if (chatHistory != this->user.chatHistories.end())
        chatHistory->second.emplace_back(chatMessage);
}

void UserLogic::CreateNewChatHistory(const std::string& peerId_)
{
    this->user.chatHistories.emplace(peerId_, ChatHistory{});
}

void UserLogic::UpdatePeerState(const std::string& peerId, const ConnectionState& state)
{
    auto result = this->user.peerDataMap.insert({peerId, {}});
    if (state == ConnectionState::Failed || state == ConnectionState::Closed || state == ConnectionState::Disconnected)
    {
        this->user.peerDataMap.erase(result.first->first);
        return;
    }
    result.first->second.connectionState = state;
}

void UserLogic::IncrementPeerUnreadMessageCount(const std::string& peerId)
{
    ++(this->user.peerDataMap.at(peerId).unreadMessageCount);
}

void UserLogic::Reset(const std::string& username)
{
    this->user = User{username};
}

const std::string& UserLogic::GetUserName() const
{
    return this->user.username;
}

void UserLogic::SendTextMessage(const std::string& message)
{
    this->chatAPI.SendMessageToPeer(this->user.selectedChat, message);
}

const std::unordered_map<std::string, PeerData>& UserLogic::GetPeerDataMap() const
{
    return this->user.peerDataMap;
}

void UserLogic::AddNewChatPeer(const std::string& peerId)
{
    this->chatAPI.AttemptToConnectToPeer(peerId);
    CreateNewChatHistory(peerId);
}

void UserLogic::SetSelectedPeerId(const std::string& peerId)
{
    this->user.selectedChat = peerId;
}

const size_t& UserLogic::GetMaxNameLength()
{
    return User::maxNameLength;
}

void UserLogic::LoginWithNewUser(const std::string& username_)
{
    Reset(username_);
    this->chatAPI.Reset();
    //this->SetUser(user_);

    this->chatAPI.SetOnPeerConnectionStateChange([this](const PeerConnectionStateChangeEvent& e) {
        UpdatePeerState(e.peerId, e.connectionState);
        if (e.connectionState == ConnectionState::Connected)
        {
            CreateNewChatHistory(e.peerId);
        }
    });
    this->chatAPI.SetOnMessageReceived([this](const ChatMessage& e) {
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
    this->chatAPI.Init(config);

    this->chatAPI.AttemptConnectionWithUsername(username_);
}

bool UserLogic::IsClientConnected() const
{
    return this->chatAPI.IsConnected();
}
