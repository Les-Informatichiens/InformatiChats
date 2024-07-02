//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChannelController.h"

#include <Model/ApplicationLogic/Command/CommandManager.h>
#include <Model/ApplicationLogic/UserLogic.h>


class ChannelController : public IChannelController
{
public:
    explicit ChannelController(UserLogic& userLogic, CommandManager& commandManager)
        : userLogic(userLogic), commandManager(commandManager){};

    ChannelViewModel GetViewModel() override;

    void AddNewChatPeer(const std::string& peerName) override;
    void SetSelectedPeerId(const std::string& peerId) override;

    void RequestUserFingerprint(const std::string& fingerprint, std::function<void(nlohmann::json)> callback) override;

private:
    UserLogic& userLogic;
    CommandManager& commandManager;
};
