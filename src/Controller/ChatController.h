//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChatController.h"
#include "Model/Model.h"

#include <string>


class ChatController : public IChatController
{
public:
    explicit ChatController(Model& model);

    ChatViewModel GetViewModel() override;

private:
    std::string userName;

    Model& model;
};
