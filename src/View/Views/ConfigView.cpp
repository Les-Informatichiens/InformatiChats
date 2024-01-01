//
// Created by thierrycd on 2023-12-31.
//

#include "ConfigView.h"

/**
 * @brief Builds a view with no panels
 */
ConfigView::ConfigView(IConfigController& controller_)
    : controller(controller_)
{
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ConfigView::ConfigView(std::vector<IPanel>& panels_, IConfigController& controller_)
    : controller(controller_)
{
    for (IPanel& panel: panels_)
    {
        this->panels.emplace_back(panel);
    }
}

void ConfigView::Draw() const
{
    for (IPanel& panel: this->panels)
    {
        panel.Draw();
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

void ConfigView::SetController(IConfigController& controller_)
{
    this->controller = controller_;
}
