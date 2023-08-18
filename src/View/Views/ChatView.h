//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Controller/IChatController.h>
#include "IView.h"

#include <memory>
#include <vector>


class ChatView : public IView {
public:
    explicit ChatView(IChatController &controller);

    ChatView(std::vector<IPanel> &panels, IChatController &controller);

    void SetController(IChatController &controller);

    void Draw() const override;

    bool IsVisible() override;

    void AddPanel(IPanel &panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
    IChatController &controller;
};
