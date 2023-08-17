#pragma once

#include <vector>
#include <memory>
#include "IView.h"
#include "Panels/IPanel.h"
#include "../Model/Chat.h"

class View: public IView {
public:
    View();
    explicit View(std::vector<IPanel>& panels);

    void Draw() const override;

    ~View() override = default;

    void AddPanel(IPanel &panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
};
