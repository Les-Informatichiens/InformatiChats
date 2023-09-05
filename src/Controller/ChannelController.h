//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChannelController.h"

#include <Model/ApplicationLogic/UserLogic.h>


class ChannelController : public IChannelController
{
public:
    explicit ChannelController(UserLogic& userLogic) : userLogic(userLogic){};

    ChannelViewModel GetViewModel() override;

    void AddNewChatPeer(const std::string& peerName) override;
    void SetSelectedPeerId(const std::string& peerId) override;

private:
    UserLogic& userLogic;
};
