//
// Created by Jean on 8/18/2023.
//
#pragma once

#include "ViewModels/ChatViewModel.h"

class IChatController
{
public:
    virtual ChatViewModel getViewModel() = 0;
};