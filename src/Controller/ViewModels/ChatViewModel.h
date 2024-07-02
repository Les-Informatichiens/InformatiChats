//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/DTO/ChannelDTO.h"
#include "Controller/DTO/RemoteUserDTO.h"
#include "Model/Models/LocalUser.h"

#include <ctime>
#include <string>

struct ChatViewModel
{
    const ChatHistory* chatHistory;
    const size_t chatHistorySize;
    // const std::string peerPermanentUsername;
    const std::string peerPublicKey;
    const std::string peerIpAddress;
    const uint64_t peerLastSeen;
    const bool isPeerContact;
    const RemoteUserDTO peerData;
};
