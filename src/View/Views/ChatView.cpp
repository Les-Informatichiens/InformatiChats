#include <Controller/IChatController.h>
#include "ChatView.h"


/**
 * @brief Builds a view with no panels
 */
ChatView::ChatView(IChatController &controller_) : controller(controller_) {
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ChatView::ChatView(std::vector<IPanel> &panels_, IChatController &controller_) : controller(controller_) {
    for (IPanel &panel: panels_) {
        this->panels.emplace_back(panel);
    }
}

/**
 * @brief Draws all the panels in the view
 */
void ChatView::Draw() const {
    for (IPanel &panel: this->panels) {
        panel.Draw();
    }
}

/**
 * @brief Adds a panel to the view
 * @param panel_ panel to be added
 */
void ChatView::AddPanel(IPanel &panel_) {
    this->panels.emplace_back(panel_);
}


/**
 * Sets the controller for the view
 * @param controller_
 */
void ChatView::SetController(IChatController &controller_) {
    this->controller = controller_;
}

bool ChatView::IsVisible() {
    return true;
}
