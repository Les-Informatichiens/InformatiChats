//
// Created by fofi1 on 2024-01-04.
//

#pragma once

#include "ICommand.h"

#include <string>

template<typename T>
class SaveNewConfigValueCommand final : public ICommand
{
public:
    SaveNewConfigValueCommand(std::string key, T value, T oldValue);
    void Execute() override;
    void Undo() override;

private:
    std::string key;
    T value;
    T oldValue;
};
