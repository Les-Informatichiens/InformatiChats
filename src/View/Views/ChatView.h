//
// Created by Jean on 8/18/2023.
//
#pragma once

#include "IView.h"
#include <Controller/ChatController.h>

#include <vector>
#include <memory>

class ChatView : public IView {
public:

    explicit ChatView(ChatController &controller);
    ChatView(std::vector<IPanel>& panels, ChatController& controller);
    void Draw() const override;

    void SetController(ChatController &controller);
    void AddPanel(IPanel &panel) override;
private:
    std::vector<std::reference_wrapper<IPanel>> panels;
    ChatController& controller;

};
