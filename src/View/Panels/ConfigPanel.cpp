//
// Created by thierrycd on 2023-12-31.
//

#include "ConfigPanel.h"
#include "imgui.h"
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>


ConfigPanel::ConfigPanel(IConfigController& controller_) : controller(controller_)
{
}

void ConfigPanel::Draw()
{

    if (IsVisible())
    {
        Update();
        // Draw the chat panel
    }
}

void ConfigPanel::Update()
{

    ImGui::OpenPopup("Configuration");
    if (ImGui::BeginPopupModal("Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {

        ImGui::EndPopup();
    }
}

bool ConfigPanel::IsVisible()
{
return false;
}
