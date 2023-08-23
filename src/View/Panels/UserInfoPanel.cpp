//
// Created by Jean on 8/17/2023.
//

#include "UserInfoPanel.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "imgui.h"


UserInfoPanel::UserInfoPanel(IChannelController& controller_) : controller(controller_) {}

void UserInfoPanel::Update()
{
    ChannelViewModel vm = controller.GetViewModel();

    if (ImGui::BeginChild("userid", ImVec2(200, ImGui::GetContentRegionAvail().y), true,
                          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::Text("%s", vm.userName.c_str());

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        auto time = glfwGetTime();
        for (double i = 0; i < 20; ++i)
        {
            ImVec2 p1;
            p1.x = ImGui::GetCursorScreenPos().x + i * 2,
            p1.y = ImGui::GetCursorScreenPos().y + 5;
            ImVec2 p2;
            p2.x = ImGui::GetCursorScreenPos().x + i * 2,
            p2.y = ImGui::GetCursorScreenPos().y + 5 * glm::sin(time + i / 3.0) + 5;
            draw_list->AddLine(p1, p2, 0xFFFFFFFF, 1);
        }
    }

    ImGui::EndChild();
    ImGui::EndGroup();
}

bool UserInfoPanel::IsVisible()
{
    return false;
}

void UserInfoPanel::Draw()
{
    Update();

    if (IsVisible())
    {
        // Draw the channels panel
    }
}
