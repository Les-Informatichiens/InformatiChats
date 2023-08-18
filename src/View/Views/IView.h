#pragma once

#include <View/Panels/IPanel.h>

class IView
{
public:
    virtual void Draw() const = 0;
    virtual ~IView() = default;
    virtual void AddPanel(IPanel& panel) = 0;
};
