//
// Created by thierrycd on 2023-12-31.
//

#include "ConfigPanel.h"
#include "imgui.h"
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>


ConfigPanel::ConfigPanel(IConfigController& controller_) : controller(controller_)
{
    this->controller.SetupConfig();
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
    ConfigViewModel vm = this->controller.GetViewModel();

    ImGui::OpenPopup("Configuration");
    if (ImGui::BeginPopupModal("Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        for (const auto& entry: this->controller.GetViewModel().entries)
        {
            ImGui::Text("%s", entry.GetName().c_str());
            ImGui::SameLine();

            if (ImGui::InputTextWithHint(("##" + entry.GetName()).c_str(), entry.GetDescription().c_str(), &this->configText, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                this->controller.SetIsCurrentlyEditingConfigs(false);
                std::cout << this->configText << std::endl;
                this->configText.clear();
            }
        }

        ImGui::EndPopup();
    }
}

bool ConfigPanel::IsVisible()
{
    return this->controller.IsCurrentlyEditingConfigs();
}
