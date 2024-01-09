//
// Created by Jean on 8/18/2023.
//

#include "LoginView.h"

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
LoginView::LoginView(std::vector<IPanel>& panels_)
{
    for (IPanel& panel: panels_)
    {
        this->panels.emplace_back(panel);
    }
}

void LoginView::Draw() const
{
    for (const std::reference_wrapper<IPanel>& panel: this->panels)
    {
        panel.get().Draw();
    }
}

void LoginView::AddPanel(IPanel& panel_)
{
    this->panels.emplace_back(panel_);
}

bool LoginView::IsVisible()
{
    return true;
}
