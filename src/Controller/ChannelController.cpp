//
// Created by Jean on 8/18/2023.
//

#include "ChannelController.h"


ChannelViewModel ChannelController::GetViewModel()
{
    return {this->model.GetPeerDataMap(), this->model.GetOwnUsername()};
}

ChannelController::ChannelController(Model& model_)
    : model(model_)
{
}

void ChannelController::AddNewChatPeer(const std::string& peerName_)
{
    this->model.AddNewChatPeer(peerName_);
}

void ChannelController::SetSelectedPeerId(const std::string& peerId_)
{
    this->model.SetSelectedPeerId(peerId_);
}