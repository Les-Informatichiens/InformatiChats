//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "../../Controller/IChannelController.h"
#include "IView.h"
#include <memory>
#include <vector>

class ChannelView : public IView
{
public:
    explicit ChannelView(IChannelController &controller);
    ChannelView(std::vector<IPanel> &panels, IChannelController &controller);
    void Draw() const override;

    void SetController(IChannelController &controller);
    void AddPanel(IPanel &panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
    IChannelController &controller;
};
