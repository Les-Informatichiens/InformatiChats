//
// Created by Jonathan Richard on 2023-10-02.
//

#pragma once

#include <string>
#include <Model/MessageDispatcher.h>
#include <Model/Models/PeerEventMessages.h>

class UserEventManager
{

public:
    void HandlePeerMessage(const std::string& peerId, const BaseMessage<MessageType>& message);

private:


};
