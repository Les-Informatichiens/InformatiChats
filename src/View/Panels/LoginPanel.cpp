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
    if (!this->controller.IsConnected())
    {
        Update();
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
            if (ImGui::Selectable(user.permanentUsername.c_str(), selectedUsername == user.permanentUsername.c_str(),ImGuiSelectableFlags_DontClosePopups))
            {
                this->selectedUsername = user.permanentUsername;
                this->isLoggingIn = true;
                ImGui::OpenPopup("Login");
            }
            ImGui::Separator();
        }

        if (ImGui::Button("Create new user..."))
        {
            ImGui::OpenPopup("Create User");
            this->controller.SetCreatingNewUser(true);
        }

        if (this->isLoggingIn)
        {
            this->isLoggingIn = false;
        }

        if (ImGui::BeginPopup("Login", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter your password for %s", selectedUsername.c_str());
            ImGui::Separator();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            bool passwordEnterPressed = ImGui::InputTextWithHint("##password", "password...", &this->passwordBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);

            if (passwordEnterPressed)
            {
                util::trim(this->passwordBuf);

                // We do not allow a user to have an empty username or a username that is only spaces
                if (!this->passwordBuf.empty())
                {
                    this->controller.LoginAttempt(selectedUsername, this->passwordBuf);
                    this->passwordBuf.clear();
                    this->isLoggingIn = false;
                    this->selectedUsername.clear();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
        else if (ImGui::BeginPopup("Create User", ImGuiWindowFlags_AlwaysAutoResize))
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
                    this->controller.CreateUser(this->usernameBuf, this->passwordBuf);
//                        this->controller.LoginAttempt(this->usernameBuf, this->passwordBuf);

                    this->usernameBuf.clear();
                    this->passwordBuf.clear();
                    this->passwordConfirmationBuf.clear();

                    this->controller.SetCreatingNewUser(false);
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::PopStyleVar();

            ImGui::EndPopup();
        }
        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
