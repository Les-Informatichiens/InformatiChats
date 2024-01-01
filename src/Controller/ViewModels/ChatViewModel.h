//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Model/Models/User.h"

#include <string>
#include <ctime>

struct ChatViewModel
{
    const ChatHistory* chatHistory;
    const size_t chatHistorySize;
    const std::string peerPermanentUsername;
    const std::string peerPublicKey;
    const std::string peerIpAddress;
    const uint64_t peerLastSeen;
    const bool isPeerContact;
    const PeerData peerData;
};
