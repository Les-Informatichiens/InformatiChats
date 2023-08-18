//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChatController.h"

#include <string>


class ChatController : public IChatController
{
public:
    ChatViewModel GetViewModel() override;

private:
    std::string userName;
};
