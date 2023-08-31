//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChatController.h"
#include <Model/ApplicationLogic/UserLogic.h>

class ChatController : public IChatController
{
public:
    explicit ChatController(UserLogic& userLogic) : userLogic(userLogic){};

    ChatViewModel GetViewModel() override;

    void SendMessage(const std::string& message) override;

private:
    UserLogic& userLogic;
};
