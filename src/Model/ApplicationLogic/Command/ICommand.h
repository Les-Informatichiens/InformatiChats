//
// Created by fofi1 on 2023-09-16.
//

#pragma once


class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual void Undo() = 0;
    virtual void Execute() = 0;
};
