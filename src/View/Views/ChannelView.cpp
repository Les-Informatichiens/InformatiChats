#include "ChannelView.h"
#include "../../Controller/ChannelController.h"

/**
 * @brief Builds a view with no panels
 */
ChannelView::ChannelView(IChannelController& controller_) : controller(controller_) {
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ChannelView::ChannelView(std::vector<IPanel>& panels_,  IChannelController& controller_):controller(controller_)
{
    for (IPanel& panel : panels_) {
        this->panels.emplace_back(panel);
    }
}

/**
 * @brief Draws all the panels in the view
 */
void ChannelView::Draw() const{
    for (IPanel& panel : this->panels) {
        panel.Draw();
    }
}

/**
 * @brief Adds a panel to the view
 * @param panel_ panel to be added
 */
void ChannelView::AddPanel(IPanel& panel_) {
    this->panels.emplace_back(panel_);
}

/**
 * Sets the controller for the view
 * @param controller_
 */
void ChannelView::SetController(IChannelController &controller_) {
    this->controller = controller_;
}


