#pragma once

#include "IPanel.h"

class ChannelPanel: IPanel {
public:
    void Draw() override;
    void Update() override;
    bool IsVisible() override;
};
