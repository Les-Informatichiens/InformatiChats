//
// Created by Jean on 8/18/2023.
//

#include "ConnexionView.h"

/**
 * @brief Builds a view with no panels
 */
ConnexionView::ConnexionView(IConnexionController &controller)
        : controller(controller) {
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
ConnexionView::ConnexionView(std::vector<IPanel>& panels_, IConnexionController &controller)
        : controller(controller)
{
    for (IPanel& panel : panels_) {
        this->panels.emplace_back(panel);
    }
}

void ConnexionView::Draw() const {
    for (IPanel& panel : this->panels) {
        panel.Draw();
    }
}

void ConnexionView::AddPanel(IPanel &panel) {
    this->panels.emplace_back(panel);
}
