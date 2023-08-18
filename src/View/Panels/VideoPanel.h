#pragma once

#include "IPanel.h"


class VideoPanel : IPanel
{
public:
    void Draw() override;

    void Update() override;

    bool IsVisible() override;
};
