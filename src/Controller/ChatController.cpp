//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


ChatViewModel ChatController::GetViewModel()
{
    if (const ChatHistory* chatHistory = this->model.GetSelectedChatHistory())
    {
        return {chatHistory, chatHistory->size()};
    }

    return {this->model.GetSelectedChatHistory(), 0};
}

ChatController::ChatController(Model& model_)
    : model(model_)
{
}

void ChatController::SendMessage(const std::string& message)
{
    this->model.SendMessage(this->model.GetSelectedPeerId(), message);
}
