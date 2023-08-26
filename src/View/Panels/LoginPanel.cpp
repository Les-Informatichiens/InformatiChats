//
// Created by Jean on 8/18/2023.
//

#include "LoginPanel.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>


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

        if (ImGui::InputTextWithHint("##username", "username...", &this->UsernameBuf,
                                     ImGuiInputTextFlags_EnterReturnsTrue))
        {
            controller.LoginAttempt(this->UsernameBuf);
        }
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
