//
// Created by Jean on 8/18/2023.
//

#include "LoginView.h"

/**
 * @brief Builds a view with no panels
 */
LoginView::LoginView(ILoginController &controller)
    : controller(controller)
{
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
LoginView::LoginView(std::vector<IPanel> &panels_, ILoginController &controller)
    : controller(controller)
{
    for (IPanel &panel: panels_)
    {
        this->panels.emplace_back(panel);
    }
}

void LoginView::Draw() const
{
    for (IPanel &panel: this->panels)
    {
        panel.Draw();
    }
}

void LoginView::AddPanel(IPanel &panel)
{
    this->panels.emplace_back(panel);
}
