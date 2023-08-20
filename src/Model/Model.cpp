//
// Created by Jean on 8/17/2023.
//

#include "Model.h"


Model::Model(IUser& user_, IChatClient& chatClient_)
: user(user_), chatClient(chatClient_)
{
}


void Model::LoginWithNewUser(const std::string& username_)
{
    this->user.Reset(username_);
    this->chatClient.Reset();
    //this->SetUser(user_);
    //this->SetChatClient(ChatClient());

    this->chatClient.SetOnPeerConnectionStateChange([this](PeerConnectionStateChangeEvent e) {
        this->user.UpdatePeerState(e.peerId, e.connectionState);
    });
    this->chatClient.SetOnMessageReceived([this](MessageReceivedEvent e) {
        this->user.IncrementPeerUnreadMessageCount(e.senderId);
    });

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "51.79.86.30";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = {stunServer, stunServerPort, signalingServer, signalingServerPort};
    this->chatClient.Init(config);

    this->chatClient.AttemptConnectionWithUsername(this->user.GetUsername());
}

void Model::AddNewChatPeer(const std::string &peerId)
{
    this->chatClient.AttemptToConnectToPeer(peerId);
}

void Model::SetUser(IUser& user_)
{
    this->user = user_;
}

void Model::SetChatClient(IChatClient& chatClient_)
{
    //this->chatClient = std::move(chatClient_);


}

std::string Model::GetOwnUsername() const
{
    return this->user.GetUsername();
}

std::unordered_map<std::string, PeerData> Model::GetPeerDataMap()
{
    return this->user.GetPeerDataMap();
}
