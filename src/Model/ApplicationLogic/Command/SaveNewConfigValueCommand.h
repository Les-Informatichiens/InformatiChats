//
// Created by fofi1 on 2024-01-04.
//

#pragma once

#include "ICommand.h"
#include <Model/ApplicationLogic/ConfigLogic.h>

#include <string>

template <typename T>
class SaveNewConfigValueCommand final : public ICommand
{
public:
    SaveNewConfigValueCommand<T>(std::string key, T value, T oldValue);
    void Execute() override;
    void Undo() override;

private:
    std::string key;
    T value;
    T oldValue;
};
