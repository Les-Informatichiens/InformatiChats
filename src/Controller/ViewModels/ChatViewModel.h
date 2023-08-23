//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Model/IUser.h>

#include <string>


struct ChatViewModel
{
    const ChatHistory* chatHistory;
    const size_t chatHistorySize;
};
