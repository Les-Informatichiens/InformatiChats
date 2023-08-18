#pragma once

class IPanel
{
public:
    virtual void Draw() = 0;

    virtual void Update() = 0;

    virtual bool IsVisible() = 0;
};
