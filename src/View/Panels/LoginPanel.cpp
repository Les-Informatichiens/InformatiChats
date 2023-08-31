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

        bool usernameEnterPressed = ImGui::InputTextWithHint("##username", "username...", &this->usernameBuf, ImGuiInputTextFlags_EnterReturnsTrue);
        bool passwordEnterPressed = ImGui::InputTextWithHint("##password", "password...", &this->passwordBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
        bool passwordConfirmationEnterPressed = ImGui::InputTextWithHint("##passwordConfirmation", "confirm password...", &this->passwordConfirmationBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);

        if (usernameEnterPressed || passwordEnterPressed || passwordConfirmationEnterPressed)
        {
            util::trim(this->usernameBuf);
            util::trim(this->passwordBuf);
            util::trim(this->passwordConfirmationBuf);

            // We do not allow a user to have an empty username or a username that is only spaces
            if (!this->usernameBuf.empty() && !this->passwordBuf.empty() && !this->passwordConfirmationBuf.empty() && this->passwordBuf == this->passwordConfirmationBuf)
            {
                controller.CreateUser(this->usernameBuf, this->passwordBuf);
                controller.LoginAttempt(this->usernameBuf, this->passwordBuf);

                this->usernameBuf.clear();
                this->passwordBuf.clear();
                this->passwordConfirmationBuf.clear();
            }
        }
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
