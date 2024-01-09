#include "ChatView.h"

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ChatView::ChatView(std::vector<IPanel>& panels_)
{
    for (IPanel& panel: panels_)
    {
        this->panels.emplace_back(panel);
    }
}

/**
 * @brief Draws all the panels in the view
 */
void ChatView::Draw() const
{
    for (const std::reference_wrapper<IPanel>& panel: this->panels)
    {
        panel.get().Draw();
    }
}

/**
 * @brief Adds a panel to the view
 * @param panel_ panel to be added
 */
void ChatView::AddPanel(IPanel& panel_)
{
    this->panels.emplace_back(panel_);
}

bool ChatView::IsVisible()
{
    return true;
}
