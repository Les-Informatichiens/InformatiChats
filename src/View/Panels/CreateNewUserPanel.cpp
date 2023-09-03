//
// Created by thierrycd on 2023-08-30.
//

#include "CreateNewUserPanel.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <util/string_util.h>


CreateNewUserPanel::CreateNewUserPanel(ILoginController& controller_)
    : controller(controller_),
      usernameBuf(),
      passwordBuf(),
      passwordConfirmationBuf()
{
}


void CreateNewUserPanel::Update()
{
//    ImGui::OpenPopup("Login");
//    if (ImGui::BeginPopup("Login", ImGuiWindowFlags_AlwaysAutoResize))
//    {
//        ImGui::Text("Enter your username");
//        ImGui::Separator();
//        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
//
//        bool usernameEnterPressed = ImGui::InputTextWithHint("##username", "username...", &this->usernameBuf, ImGuiInputTextFlags_EnterReturnsTrue);
//        bool passwordEnterPressed = ImGui::InputTextWithHint("##password", "password...", &this->passwordBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
//        bool passwordConfirmationEnterPressed = ImGui::InputTextWithHint("##passwordConfirmation", "confirm password...", &this->passwordConfirmationBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
//
//        if (usernameEnterPressed || passwordEnterPressed || passwordConfirmationEnterPressed)
//        {
//            util::trim(this->usernameBuf);
//            util::trim(this->passwordBuf);
//            util::trim(this->passwordConfirmationBuf);
//
//            // We do not allow a user to have an empty username or a username that is only spaces
//            if (!this->usernameBuf.empty() && !this->passwordBuf.empty() && !this->passwordConfirmationBuf.empty() && this->passwordBuf == this->passwordConfirmationBuf)
//            {
//                this->controller.CreateUser(this->usernameBuf, this->passwordBuf);
//                this->controller.LoginAttempt(this->usernameBuf, this->passwordBuf);
//
//                this->usernameBuf.clear();
//                this->passwordBuf.clear();
//                this->passwordConfirmationBuf.clear();
//
//                this->controller.SetCreatingNewUser(false);
//            }
//        }
//
//        ImGui::PopStyleVar();
//
//        ImGui::EndPopup();
//    }
}

bool CreateNewUserPanel::IsVisible()
{
    return this->controller.IsCreatingNewUser();
}

void CreateNewUserPanel::Draw()
{
    if (this->IsVisible())
    {
        this->Update();
    }
}
