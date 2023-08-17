#pragma once

#include <vector>
#include <memory>
#include "IView.h"
#include "Panels/IPanel.h"

class View: IView {
public:
    View();
    explicit View(std::vector<IPanel>& panels);

    void Draw() override;
    void AddPanel(IPanel& panel) override;

private:
    std::vector<std::reference_wrapper<IPanel>> panels;
};
