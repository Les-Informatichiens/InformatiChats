#include "ChannelView.h"

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ChannelView::ChannelView(std::vector<IPanel>& panels_)
{
    for (IPanel& panel: panels_)
    {
        this->panels.emplace_back(panel);
    }
}

/**
 * @brief Draws all the panels in the view
 */
void ChannelView::Draw() const
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
void ChannelView::AddPanel(IPanel& panel_)
{
    this->panels.emplace_back(panel_);
}

bool ChannelView::IsVisible()
{
    return true;
}
