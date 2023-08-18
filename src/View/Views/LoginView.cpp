//
// Created by Jean on 8/18/2023.
//

#include <Controller/ILoginController.h>
#include "LoginView.h"


/**
 * @brief Builds a view with no panels
 */
LoginView::LoginView(ILoginController &controller_)
        : controller(controller_) {
}

/**
 * @brief Builds a view with the given panels
 * @param panels_ panels to be added to the view
 */
LoginView::LoginView(std::vector<IPanel> &panels_, ILoginController &controller_)
        : controller(controller_) {
    for (IPanel &panel: panels_) {
        this->panels.emplace_back(panel);
    }
}

void LoginView::Draw() const {
    for (IPanel &panel: this->panels) {
        panel.Draw();
    }
}

void LoginView::AddPanel(IPanel &panel_) {
    this->panels.emplace_back(panel_);
}

bool LoginView::IsVisible() {
    return true;
}

void LoginView::SetController(ILoginController &controller_) {
    this->controller = controller_;
}
