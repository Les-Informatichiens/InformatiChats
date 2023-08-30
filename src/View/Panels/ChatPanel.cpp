#include "ChatPanel.h"
#include "Model/Models/User.h"

#include "imgui.h"
#include <iomanip>
#include <misc/cpp/imgui_stdlib.h>
#include <sstream>
#include <util/string_util.h>


std::string formatMilliseconds(std::chrono::milliseconds ms)
{
    // Convert epoch time to a time_point
    std::chrono::system_clock::time_point timePoint = std::chrono::time_point<std::chrono::system_clock>(ms);

    // Convert time_point to a time structure (tm)
    std::time_t rawTime = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* timeInfo = std::localtime(&rawTime);

    // Format the time structure to a string
    std::stringstream buffer;
    buffer << std::put_time(timeInfo, "%H:%M:%S");

    return buffer.str();
}

ChatPanel::ChatPanel(IChatController& controller_) : controller(controller_) {}

void ChatPanel::Update()
{
    ChatViewModel vm = controller.GetViewModel();

    ImGui::SameLine();
    //    ImGui::BeginDisabled(vm.chatHistory == nullptr);

    if (ImGui::BeginChild("Chat", ImVec2(ImGui::GetContentRegionAvail().x, 0), true, ImGuiWindowFlags_AlwaysAutoResize))
    {

        bool reclaim_focus = false;

        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            bool paleBackground = false;

            auto chatHistory = vm.chatHistory;

            if (chatHistory)
            {
                if (ImGui::BeginTable("Console", 2, ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthStretch);

                    for (int i = 0; i < vm.chatHistorySize; ++i)
                    {
                        std::string timestamp = formatMilliseconds(chatHistory->at(i).timestamp);

                        ImGui::TableNextRow();
                        ImGui::TableNextRow();
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImColor(ImGui::GetStyle().Colors[paleBackground ? ImGuiCol_TableRowBg : ImGuiCol_TableRowBgAlt]));

                        ImGui::TableNextColumn();
                        ImGui::TextDisabled("%s", timestamp.c_str());

                        ImGui::TableNextColumn();
                        ImGui::PushStyleColor(0, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
                        ImGui::Text("[ %s ]", chatHistory->at(i).senderId.c_str());
                        ImGui::PopStyleColor();

                        ImGui::TableNextRow();
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImColor(ImGui::GetStyle().Colors[paleBackground ? ImGuiCol_TableRowBg : ImGuiCol_TableRowBgAlt]));
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn();
                        ImGui::TextWrapped("%s", chatHistory->at(i).content.c_str());

                        paleBackground = !paleBackground;
                    }
                    ImGui::EndTable();
                }
            }

            // Continue scrolling to the bottom if we're at the bottom
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            {
                ImGui::SetScrollHereY(1.0f);
            }
        }

        ImGui::EndChild();


        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
                                               ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", &inputBuf, input_text_flags))
        {
            util::trim(inputBuf);

            if (!inputBuf.empty())
            {
                this->controller.SendMessage(inputBuf);
            }

            inputBuf.clear();

            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1);// Auto focus previous widget
    }
    ImGui::EndChild();
    //    ImGui::EndDisabled();
}

void ChatPanel::Draw()
{
    Update();

    if (IsVisible())
    {
        // Draw the chat panel
    }
}

bool ChatPanel::IsVisible()
{
    return false;
}
