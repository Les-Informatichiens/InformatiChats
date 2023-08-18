//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IView.h"
#include <Controller/ChannelController.h>

#include <vector>
#include <memory>

class ChannelView : public IView{
public:
    explicit ChannelView(ChannelController &controller);
    ChannelView(std::vector<IPanel>& panels, ChannelController& controller);
    void Draw() const override;

    void SetController(ChannelController &controller);
    void AddPanel(IPanel &panel) override;
private:
    std::vector<std::reference_wrapper<IPanel>> panels;
    ChannelController& controller;
};
