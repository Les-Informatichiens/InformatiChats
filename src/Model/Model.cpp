//
// Created by Jean on 8/17/2023.
//

#include "Model.h"


Model::Model()
    : user(nullptr), chatClient(nullptr)
{
}

void Model::LoginWithNewUser(std::unique_ptr<User> user_)
{
    this->SetUser(std::move(user_));
    this->SetChatClient(std::make_unique<ChatClient>());

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "51.79.86.30";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = {stunServer, stunServerPort, signalingServer, signalingServerPort};
    this->chatClient->Init(config);

    this->chatClient->AttemptConnectionWithUsername(this->user->GetUsername());
}

void Model::SetUser(std::unique_ptr<User> user_)
{
    this->user.reset(nullptr);
    this->user = std::move(user_);
}

void Model::SetChatClient(std::unique_ptr<ChatClient> chatClient_)
{
    this->chatClient.reset(nullptr);
    this->chatClient = std::move(chatClient_);
}

std::string Model::GetOwnUsername() const
{
    if(this->user != nullptr)
    {
        return this->user->GetUsername();
    }

    return {};
}
