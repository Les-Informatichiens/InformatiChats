//
// Created by thierrycd on 2023-12-31.
//

#include "ConfigController.h"

ConfigController::ConfigController(UserLogic& userLogic_, CommandManager& commandManager_)
    : userLogic(userLogic_), commandManager(commandManager_){};

ConfigViewModel ConfigController::GetViewModel()
{
    return ConfigViewModel(ConfigLogic::GetInstance().GetConfigs());
}

void ConfigController::SetupConfig() const
{
    ConfigLogic::GetInstance().SetupConfig();
}

void ConfigController::SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_)
{
    ConfigLogic::GetInstance().SetIsCurrentlyEditingConfigs(currentlyEditingConfigs_);
}

bool ConfigController::IsCurrentlyEditingConfigs() const
{
    return ConfigLogic::GetInstance().IsCurrentlyEditingConfigs();
}
