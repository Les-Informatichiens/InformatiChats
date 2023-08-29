//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


ChatViewModel ChatController::GetViewModel()
{
    const ChatHistory* chatHistory = this->userLogic.GetSelectedChatHistory();

    if (chatHistory)
    {
        return {chatHistory, chatHistory->size()};
    }

    return {nullptr, 0};
}

ChatController::ChatController(IUserLogic& userLogic)
    : userLogic(userLogic)
{
}

void ChatController::SendMessage(const std::string& message)
{
    this->userLogic.SendMessageToPeer(message);

    this->userLogic.AddChatMessageToSelectedChatHistory(message);
}
