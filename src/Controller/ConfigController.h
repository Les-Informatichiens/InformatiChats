//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include "IConfigController.h"
#include "Model/ApplicationLogic/Command/CommandManager.h"
#include "Model/ApplicationLogic/ConfigLogic.h"
#include "Model/ApplicationLogic/UserLogic.h"

class ConfigController : public IConfigController
{
public:
    ConfigController(UserLogic& userLogic_, CommandManager& commandManager_);

    ConfigViewModel GetViewModel() override;
    void SetupConfig() const override;

    void SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_) override;
    [[nodiscard]] bool IsCurrentlyEditingConfigs() const override;

private:
    UserLogic& userLogic;
    CommandManager& commandManager;
};
