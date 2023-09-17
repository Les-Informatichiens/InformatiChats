//
// Created by fofi1 on 2023-09-16.
//

#include "CommandManager.h"


/**
 * @brief Executes command and adds it to history
 * @param command command to execute
 */
void CommandManager::ExecuteCommand(std::shared_ptr<ICommand>&& command)
{
    command->Execute();
    this->commandHistory.push(std::move(command));
}

/**
 * @brief Undo last command and adds it to undo history
 */
void CommandManager::UndoCommand()
{
    if (this->commandHistory.empty())
    {
        return;
    }

    std::shared_ptr<ICommand>& command = this->commandHistory.top();
    command->Undo();
    this->commandUndoHistory.push(std::move(command));
    this->commandHistory.pop();
}

/**
 * @brief Redo last command and adds it to command history
 */
void CommandManager::RedoCommand()
{
    if (this->commandUndoHistory.empty())
    {
        return;
    }

    std::shared_ptr<ICommand>& command = this->commandUndoHistory.top();
    command->Execute();
    this->commandHistory.push(std::move(command));
    this->commandUndoHistory.pop();
}
