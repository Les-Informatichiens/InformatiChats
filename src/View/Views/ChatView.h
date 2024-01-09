//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IView.h"

#include <functional>
#include <vector>


class ChatView final : public IView
{
public:
    ChatView() = default;
    explicit ChatView(std::vector<IPanel>& panels);

    void Draw() const override;
    bool IsVisible() override;
    void AddPanel(IPanel& panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
};
