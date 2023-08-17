#pragma once

#include "IPanel.h"

class ChatPanel: IPanel {
public:
    void Draw() override;
    void Update() override;
    bool IsVisible() override;
};
