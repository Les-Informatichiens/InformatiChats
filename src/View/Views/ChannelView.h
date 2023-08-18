//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IView.h"
#include <Controller/IChannelController.h>

#include <memory>
#include <vector>


class ChannelView : public IView
{
public:
    explicit ChannelView(IChannelController &controller);

    ChannelView(std::vector<IPanel> &panels, IChannelController &controller);

    void SetController(IChannelController &controller);

    void Draw() const override;

    bool IsVisible() override;

    void AddPanel(IPanel &panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
    IChannelController &controller;
};
