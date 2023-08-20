//
// Created by patri on 2023-08-20.
//

#pragma once

#include "View/Views/IView.h"
#include "Model/Model.h"

class AbstractViewFactory
{
public:
    virtual IView CreateView(Model& model) = 0;
};