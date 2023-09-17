//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once
#include <chrono>
#include <string>

struct ChatEntry
{
    ChatEntry(ChatMessage&& chatMessage, const std::string& senderId)
        : content(std::move(chatMessage.content))
    {
    }

    std::string content;
    std::chrono::milliseconds timestamp;
    std::string senderId;
};
