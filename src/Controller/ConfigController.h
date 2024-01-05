//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "IConfigController.h"
#include "Model/ApplicationLogic/Command/CommandManager.h"
#include "Model/ApplicationLogic/UserLogic.h"

class ConfigController : public IConfigController
{
public:
    ConfigController(UserLogic& userLogic_, CommandManager& commandManager_);

private:
    UserLogic& userLogic;
    CommandManager& commandManager;
};
