//
// Created by fofi1 on 2023-09-16.
//

#pragma once


/**
 * @brief Interface for command pattern
 */
class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual void Execute() = 0;
    virtual void Undo() = 0;
};
