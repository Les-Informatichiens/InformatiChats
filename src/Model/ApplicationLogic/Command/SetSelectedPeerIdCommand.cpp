//
// Created by fofi1 on 2023-09-16.
//

#include "SetSelectedPeerIdCommand.h"


/**
 * @brief Changes selected peer id in model, swaps old and new peer id
 */
void SetSelectedPeerIdCommand::Execute()
{
    this->userLogic.SetSelectedPeerId(this->peerId);

    std::string tempId = this->oldPeerId;
    this->oldPeerId = this->peerId;
    this->peerId = std::move(tempId);
}

/**
 * @brief Changes selected peer id in model, swaps old and new peer id
 */
void SetSelectedPeerIdCommand::Undo()
{
    this->Execute();
}
