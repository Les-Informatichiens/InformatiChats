//
// Created by Jean on 8/18/2023.
//

#include "LoginPanel.h"
#include <iostream>


LoginPanel::LoginPanel(ILoginController& controller_) : controller(controller_)
{
    this->controller.LoadLocalUsers();
}


void LoginPanel::Draw()
{
    Update();

    if (!this->controller.IsConnected())
    {
        // Draw the chat panel
    }
}

void LoginPanel::Update()
{
    //LoginViewModel vm = this->controller.GetViewModel();
    ImGui::OpenPopup("Local Users");
    if (ImGui::BeginPopupModal("Local Users", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        for (const auto& user: this->controller.GetViewModel().localUserInfos)
        {
            ImGui::Text("%s", user.permanentUsername.c_str());
            ImGui::Separator();
        }

        if (ImGui::Button("Create new user..."))
        {
            this->controller.SetCreatingNewUser(true);
        }

        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
