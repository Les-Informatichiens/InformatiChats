//
// Created by Jean on 8/18/2023.
//
#pragma once

#include <vector>
#include <memory>
#include "IView.h"
#include "../../Controller/IChatController.h"

class ChatView : public IView {
public:

    explicit ChatView(IChatController &controller);
    ChatView(std::vector<IPanel>& panels, IChatController& controller);
    void Draw() const override;

    void SetController(IChatController &controller);
    void AddPanel(IPanel &panel) override;
private:
    std::vector<std::reference_wrapper<IPanel>> panels;
    IChatController& controller;

};
