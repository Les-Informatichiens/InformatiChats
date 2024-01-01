//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once


#include "ChatMessage.h"
#include <chrono>
#include <string>

struct ChatMessageInfo
{
    std::string content;
    std::chrono::milliseconds timestamp;
    std::string senderId;
};
