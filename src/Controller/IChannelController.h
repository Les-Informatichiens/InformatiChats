//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Controller/ViewModels/ChannelViewModel.h>

#include <string>


class IChannelController
{
public:
    virtual ChannelViewModel GetViewModel() = 0;

    virtual void AddNewChatPeer(const std::string& peerName) = 0;

    virtual void SetSelectedPeerId(const std::string& peerId) = 0;
};
