//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include <string>

#include <Model/Models/ChatEntry.h>

#include <functional>

class ITextChatAPI
{
public:
    virtual void InitiateTextChat(const std::string& peerId) = 0;
    virtual void CloseTextChat(const std::string& peerId) = 0;
    virtual void SendMessageToPeer(const std::string& peerId, const std::string& message) = 0;
    virtual void OnChatMessage(std::function<void(ChatEntry)> callback) = 0;
};
