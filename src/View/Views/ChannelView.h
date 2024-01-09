//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IView.h"

#include <functional>
#include <vector>


class ChannelView final : public IView
{
public:
    ChannelView() = default;
    explicit ChannelView(std::vector<IPanel>& panels);

    void Draw() const override;
    bool IsVisible() override;
    void AddPanel(IPanel& panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
};
