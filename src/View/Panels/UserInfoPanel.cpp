//
// Created by Jean on 8/17/2023.
//

#include "UserInfoPanel.h"
#include <iostream>


#include <iostream>
#include <ostream>

UserInfoPanel::UserInfoPanel(IUserController& controller_) : controller(controller_) {}

void UserInfoPanel::Update()
{
    UserViewModel vm = this->controller.GetViewModel();

    if (ImGui::BeginChild("userid", ImVec2(200, ImGui::GetContentRegionAvail().y), true,
                          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::PushStyleColor(0, ImVec4{vm.nameColor[0], vm.nameColor[1], vm.nameColor[2], 1.0f});
        ImGui::Text("%s", vm.username.c_str());
        ImGui::PopStyleColor();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::SameLine();
        if (ImGui::Button("Edit"))
        {
            this->displayNameBuf = vm.displayName;
            this->descriptionBuf = vm.description;
            this->statusBuf = vm.status;
            this->colorBuf[0] = vm.nameColor[0];
            this->colorBuf[1] = vm.nameColor[1];
            this->colorBuf[2] = vm.nameColor[2];
            std::cout << displayNameBuf << std::endl;
            ImGui::OpenPopup("Edit User Profile");
        }
        if (ImGui::BeginPopup("Edit User Profile", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Edit User Profile");
            ImGui::Separator();

            ImGui::Text("Display Name");
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::PushStyleColor(0, ImVec4{this->colorBuf[0], this->colorBuf[1], this->colorBuf[2], 1.0f});
            ImGui::InputTextWithHint("##displayname", "display name...", &this->displayNameBuf, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::Text("Description");
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::InputTextWithHint("##description", "description...", &this->descriptionBuf, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar();

            ImGui::Text("Status");
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::InputTextWithHint("##status", "status...", &this->statusBuf, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar();

            ImGui::Text("Name Color");
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::ColorEdit3("##namecolor", &this->colorBuf[0], ImGuiColorEditFlags_NoInputs);
            ImGui::PopStyleVar();

            if (ImGui::Button("Save"))
            {
                this->controller.UpdateUserDisplayName(this->displayNameBuf);
                this->controller.UpdateUserDescription(this->descriptionBuf);
                this->controller.UpdateUserStatus(this->statusBuf);
                this->controller.UpdateUserNameColor({this->colorBuf[0], this->colorBuf[1], this->colorBuf[2]});
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }


        auto time = clock();
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
