//
// Created by Jean on 8/17/2023.
//

#include "Model.h"

#include <chrono>


Model::Model(IUser& user_, IChatAPI& chatClient_)
    : user(user_), chatClient(chatClient_)
{
}


void Model::LoginWithNewUser(const std::string& username_)
{
    this->user.Reset(username_);
    this->chatClient.Reset();
    //this->SetUser(user_);

    this->chatClient.SetOnPeerConnectionStateChange([this](const PeerConnectionStateChangeEvent& e) {
        this->user.UpdatePeerState(e.peerId, e.connectionState);
        if (e.connectionState == ConnectionState::Connected)
        {
            this->user.CreateNewChatHistory(e.peerId);
        }
    });
    this->chatClient.SetOnMessageReceived([this](const MessageReceivedEvent& e) {
        this->user.IncrementPeerUnreadMessageCount(e.senderId);
        this->user.AddChatMessageToPeerChatHistory(e.senderId, {e.content,
                                                                duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                                                                e.senderId});
    });

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "informatichiens.com";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = {stunServer, stunServerPort, signalingServer, signalingServerPort};
    this->chatClient.Init(config);

    this->chatClient.AttemptConnectionWithUsername(this->user.GetUsername());
}

void Model::AddNewChatPeer(const std::string& peerId)
{
    this->chatClient.AttemptToConnectToPeer(peerId);
    this->user.CreateNewChatHistory(peerId);
}

void Model::SetUser(IUser& user_)
{
    this->user = user_;
}

void Model::SetChatClient(IChatAPI& chatClient_)
{
    //this->chatClient = std::move(chatClient_);
}

std::string Model::GetOwnUsername() const
{
        return }

std::unordered_map<std::string, PeerData> Model::GetPeerDataMap()
{
    return this->user.GetPeerDataMap();
}

void Model::SetSelectedPeerId(const std::string& peerId_)
{
    this->user.SetSelectedPeerId(peerId_);
}
