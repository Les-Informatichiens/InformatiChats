//
// Created by Jean on 8/18/2023.
//

#include "LoginPanel.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <util/string_util.h>


LoginPanel::LoginPanel(ILoginController& controller_) : controller(controller_)
{
}


void LoginPanel::Draw()
{

    if (!controller.IsConnected())
    {
        Update();
        // Draw the chat panel
    }
}

void LoginPanel::Update()
{

    LoginViewModel vm = controller.GetViewModel();


    ImGui::OpenPopup("Login");
    if (ImGui::BeginPopupModal("Login", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter your username");
        ImGui::Separator();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        bool usernameEnterPressed = ImGui::InputTextWithHint("##username", "username...", &this->usernameBuf, ImGuiInputTextFlags_EnterReturnsTrue);
        bool passwordEnterPressed = ImGui::InputTextWithHint("##password", "password...", &this->passwordBuf, ImGuiInputTextFlags_EnterReturnsTrue);
        if (usernameEnterPressed || passwordEnterPressed)
        {
            util::trim(this->usernameBuf);
            util::trim(this->passwordBuf);

            // We do not allow a user to have an empty username or a username that is only spaces
            if (!this->usernameBuf.empty() && !this->passwordBuf.empty())
            {
                controller.CreateUser(this->usernameBuf, this->passwordBuf);
                controller.LoginAttempt(this->usernameBuf, this->passwordBuf);
            }

            this->usernameBuf.clear();
            this->passwordBuf.clear();
        }
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
