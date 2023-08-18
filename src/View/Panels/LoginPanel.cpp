//
// Created by Jean on 8/18/2023.
//

#include "LoginPanel.h"
#include "imgui.h"


LoginPanel::LoginPanel(ILoginController &controller) : controller(controller)
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

    LoginViewModel vm = controller.getViewModel();


    ImGui::OpenPopup("Login");
    if (ImGui::BeginPopupModal("Login", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter your username");
        ImGui::Separator();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        std::string buf;
        if (ImGui::InputTextWithHint("##username", "otisma...", buf.data(), vm.maxNameLength, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            controller.AttemptConnectionWithUsername(buf);
        }
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }
}

bool LoginPanel::IsVisible()
{
    return false;
}
