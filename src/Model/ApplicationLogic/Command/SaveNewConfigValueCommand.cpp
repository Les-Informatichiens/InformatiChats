//
// Created by fofi1 on 2024-01-04.
//

#include "SaveNewConfigValueCommand.h"

template <typename T>
SaveNewConfigValueCommand<T>::SaveNewConfigValueCommand(std::string key, T value, T oldValue)
: key(std::move(key)), value(value), oldValue(oldValue)
{
}

template <typename T>
void SaveNewConfigValueCommand<T>::Execute()
{
    ConfigLogic::SaveConfigValue(key, value);
}

template <typename T>
void SaveNewConfigValueCommand<T>::Undo()
{
    ConfigLogic::SaveConfigValue(key, oldValue);
}