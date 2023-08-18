//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Controller/ViewModels/ChatViewModel.h>


class IChatController
{
public:
    virtual ChatViewModel GetViewModel() = 0;
};