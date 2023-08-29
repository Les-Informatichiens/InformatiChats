//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChatController.h"
#include "Model/Application-Logic/UserLogic.h"
#include "Model/Model.h"

#include <string>


class ChatController : public IChatController
{
public:
    explicit ChatController(UserLogic& userLogic);

    ChatViewModel GetViewModel() override;

    void SendMessage(const std::string& message) override;

private:
    UserLogic& userLogic;
};
