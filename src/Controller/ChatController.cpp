//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


ChatViewModel ChatController::GetViewModel()
{
    auto chatHistory = userLogic.GetSelectedChatHistory();

    if (chatHistory)
    {
        return {chatHistory, chatHistory->size()};
    }
    return {nullptr, 0};
}

void ChatController::SendMessage(const std::string& message)
{
    this->userLogic.SendTextMessage(message);

    this->userLogic.AppendSelectedChatHistory(message);
}
