//
// Created by fofi1 on 2023-09-16.
//

#pragma once

#include "ICommand.h"
#include <Model/ApplicationLogic/UserLogic.h>
#include <string>
#include <utility>


/**
 * @brief Command for setting selected peer id
 */
class SetSelectedPeerIdCommand : public ICommand
{
public:
    SetSelectedPeerIdCommand(UserLogic& userLogic_, std::string oldPeerId, std::string peerId)
        : userLogic(userLogic_), oldPeerId(std::move(oldPeerId)), peerId(std::move(peerId)){};

    void Execute() override;
    void Undo() override;

private:
    UserLogic& userLogic;

    std::string oldPeerId;
    std::string peerId;
};
