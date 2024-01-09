//
// Created by thierrycd on 2023-12-31.
//

#include "ConfigView.h"


/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ConfigView::ConfigView(std::vector<IPanel>& panels_)
{
    for (IPanel& panel: panels_)
    {
        this->panels.emplace_back(panel);
    }
}

void ConfigView::Draw() const
{
    for (const std::reference_wrapper<IPanel>& panel: this->panels)
    {
        panel.get().Draw();
    }
}

void ConfigView::AddPanel(IPanel& panel_)
{
    this->panels.emplace_back(panel_);
}

bool ConfigView::IsVisible()
{
    return true;
}
