//
// Created by Jean on 8/18/2023.
//

#include "LoginPanel.h"


LoginPanel::LoginPanel(ILoginController& controller_) : controller(controller_)
{
}


void LoginPanel::Draw()
{

    if (!this->controller.IsConnected())
    {
        Update();
        // Draw the chat panel
    }
}

void LoginPanel::Update()
{

    //LoginViewModel vm = this->controller.GetViewModel();


    ImGui::OpenPopup("Login");
    if (ImGui::BeginPopupModal("Login", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter your username");
        ImGui::Separator();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        if (ImGui::InputTextWithHint("##username", "username...", &this->usernameBuf,
                                     ImGuiInputTextFlags_EnterReturnsTrue))
        {
            util::trim(this->usernameBuf);

            // We do not allow a user to have an empty username or a username that is only spaces
            if (!this->usernameBuf.empty())
            {
                this->controller.LoginAttempt(this->usernameBuf);
            }

            this->usernameBuf.clear();
        }
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
