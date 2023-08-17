#include "View.h"

/**
 * @brief Builds a view with no panels
 */
View::View()
: panels()
{
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
View::View(std::vector<IPanel>& panels_)
{
    for (IPanel& panel : panels_) {
        this->panels.emplace_back(panel);
    }
}
/**
 * @brief Draws all the panels in the view
 */
void View::Draw() const{
    for (IPanel& panel : this->panels) {
        panel.Draw();
    }
}
/**
 * @brief Adds a panel to the view
 * @param panel_ panel to be added
 */

void View::AddPanel(IPanel& panel) {
    this->panels.emplace_back(panel);
}

