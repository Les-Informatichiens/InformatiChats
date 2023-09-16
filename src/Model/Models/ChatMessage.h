//
// Created by Jean on 8/29/2023.
#pragma once

#include <chrono>
#include <string>

struct ChatMessage
{
    std::string content;
    std::chrono::milliseconds timestamp;
    std::string senderId;
};
