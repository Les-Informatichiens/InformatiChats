//
// Created by Jean on 8/18/2023.
//

#include "ChannelController.h"
#include <Model/ApplicationLogic/Command/SetSelectedPeerIdCommand.h>

#include <memory>


ChannelViewModel ChannelController::GetViewModel()
{
    return {this->userLogic.GetPeerDataMap(),
            this->userLogic.GetSelectedPeerId(),
            this->userLogic.GetUserName()};
}

void ChannelController::AddNewChatPeer(const std::string& peerName_)
{
    this->userLogic.AddNewChatPeer(peerName_);
}

void ChannelController::SetSelectedPeerId(const std::string& peerId_)
{
    this->commandManager.ExecuteCommand(std::make_shared<SetSelectedPeerIdCommand>(
            this->userLogic,
            this->userLogic.GetSelectedPeerId(),
            peerId_));
}
