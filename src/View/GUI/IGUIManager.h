//
// Created by Jonathan Richard on 2023-08-23.
//

#pragma once

#include "View/Backend/IWindow.h"

class IGUIManager
{
public:
    virtual bool Init() = 0;
    virtual void NewFrame() = 0;
    virtual void RenderFrame() = 0;
    virtual void SetDisplayResolutionFactor(float resFactor) = 0;
    virtual void Uninit() = 0;
};
