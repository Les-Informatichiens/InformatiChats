//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "Model/Models/ChatMessage.h"


#include <functional>
#include <string>

#include <Model/Models/ChatMessageInfo.h>

#include <functional>

class ITextChatAPI
{
public:
    virtual void InitiateTextChat(const std::string& peerId) = 0;
    virtual void CloseTextChat(const std::string& peerId) = 0;

    virtual void SendMessageToPeer(const std::string& peerId, const std::string& destinationChannel, const std::string& message) = 0;

    virtual void OnChatMessage(std::function<void(ChatMessageInfo)> callback) = 0;
    virtual void OnChatMessage(const std::string& peerId, std::function<void(ChatMessage)> callback) = 0;
};
