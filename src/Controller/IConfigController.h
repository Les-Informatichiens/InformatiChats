//
// Created by thierrycd on 2023-12-31.
//

#pragma once

#include <Controller/ViewModels/ConfigViewModel.h>

class IConfigController
{

public:
    virtual void SetupConfig() const = 0;
    virtual ConfigViewModel GetViewModel() = 0;

    virtual void SetIsCurrentlyEditingConfigs(bool currentlyEditingConfigs_) = 0;
    [[nodiscard]] virtual bool IsCurrentlyEditingConfigs() const = 0;
};
