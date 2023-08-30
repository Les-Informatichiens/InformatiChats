//
// Created by Jean on 8/18/2023.
//

#include "ChannelController.h"


ChannelViewModel ChannelController::GetViewModel()
{
    return {userLogic.GetPeerDataMap(), userLogic.GetUserName()};
}

void ChannelController::AddNewChatPeer(const std::string& peerName_)
{
    userLogic.AddNewChatPeer(peerName_);
}

void ChannelController::SetSelectedPeerId(const std::string& peerId_)
{
    userLogic.SetSelectedPeerId(peerId_);
}
