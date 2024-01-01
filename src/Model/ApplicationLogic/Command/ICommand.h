//
// Created by thierrycd on 2023-09-16.
//

#pragma once


/**
 * @brief Interface for command pattern
 */
class ICommand
{
public:
    virtual void Execute() = 0;
    virtual void Undo() = 0;
};
