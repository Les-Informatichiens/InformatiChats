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

    ChatViewModel GetViewModel() override;

    void SendMessage(const std::string& message) override;
    void AddPeerAsContact(const std::string& peerId) override;
    void RemovePeerFromContacts(const std::string& peerId) override;

private:
    UserLogic& userLogic;
    CommandManager& commandManager;
};
