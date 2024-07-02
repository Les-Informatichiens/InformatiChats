//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChatController.h"
#include <Model/ApplicationLogic/Command/CommandManager.h>
#include <Model/ApplicationLogic/UserLogic.h>


class ChatController : public IChatController
{
public:
    explicit ChatController(UserLogic& userLogic, CommandManager& commandManager)
        : userLogic(userLogic), commandManager(commandManager){};

    ChatViewModel GetViewModel(const std::string& selectedChannel) override;

    void SendMessage(const std::string& destinationChannel, const std::string& message) override;
    void AddPeerAsContact(const std::string& peerId) override;
    void RemovePeerFromContacts(const std::string& peerId) override;
    void AddUserToChannel(const std::string& fingerprint, const std::string& channelUuid) override;
    ChannelDTO GetChannelDTO(const std::string& channelUuid) override;

private:
    UserLogic& userLogic;
    CommandManager& commandManager;
};
