//
// Created by fofi1 on 2024-01-04.
//

#include "SaveNewConfigValueCommand.h"
#include "Model/Models/ConfigFile.hpp"

template <typename T>
SaveNewConfigValueCommand<T>::SaveNewConfigValueCommand(std::string key, T value, T oldValue)
: key(std::move(key)), value(std::move(value)), oldValue(std::move(oldValue))
{
}

template <typename T>
void SaveNewConfigValueCommand<T>::Execute()
{
    ConfigFile config(config::CONFIG_FILE_NAME);
    config.SaveValue(this->key, this->value);
}

template <typename T>
void SaveNewConfigValueCommand<T>::Undo()
{
    ConfigFile config(config::CONFIG_FILE_NAME);
    config.SaveValue(this->key, this->oldValue);
}