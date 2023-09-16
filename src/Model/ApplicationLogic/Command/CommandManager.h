//
// Created by fofi1 on 2023-09-16.
//

#pragma once

#include "ICommand.h"
#include <memory>
#include <stack>


class CommandManager
{
public:
    void ExecuteCommand(std::shared_ptr<ICommand> command);
    void UndoCommand();
    void RedoCommand();

private:
    std::stack<std::shared_ptr<ICommand>> commandHistory;
    std::stack<std::shared_ptr<ICommand>> commandUndoHistory;
};
