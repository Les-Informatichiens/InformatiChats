//
// Created by thierrycd on 2023-12-31.
//

#include "ConfigController.h"

ConfigController::ConfigController(UserLogic& userLogic_, ConfigLogic& configLogic_, CommandManager& commandManager_)
    : userLogic(userLogic_), configLogic(configLogic_), commandManager(commandManager_){};

ConfigViewModel ConfigController::GetViewModel()
{
    return ConfigViewModel(configLogic.GetConfigs());
}

void ConfigController::SetupConfig() const
{
    this->configLogic.SetupConfig();
}

void ConfigController::SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_)
{
    this->configLogic.SetIsCurrentlyEditingConfigs(currentlyEditingConfigs_);
}

bool ConfigController::IsCurrentlyEditingConfigs() const
{
    return this->configLogic.IsCurrentlyEditingConfigs();
}
