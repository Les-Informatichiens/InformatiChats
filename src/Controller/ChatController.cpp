//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


ChatViewModel ChatController::GetViewModel()
{
    return {this->model.GetSelectedChatHistory()};
}

ChatController::ChatController(Model& model_)
    : model(model_)
{
}

void ChatController::SendMessage(const std::string& message)
{
    this->model.SendMessage(this->model.GetSelectedPeerId(), message);
}
