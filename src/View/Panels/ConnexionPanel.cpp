//
// Created by Jean on 8/18/2023.
//

#include "ConnexionPanel.h"
#include "imgui.h"


ConnexionPanel::ConnexionPanel(IConnexionController &controller) : controller(controller)
{
}


void ConnexionPanel::Draw()
{

    if (!controller.IsConnected())
    {
        Update();
        // Draw the chat panel
    }
}

void ConnexionPanel::Update()
{

    ConnexionViewModel vm = controller.getViewModel();


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

bool ConnexionPanel::IsVisible()
{
    return false;
}
