#include "ChatPanel.h"


#include "View/Event/SetSelectedChannelEvent.h"


#include <imgui_internal.h>

std::pair<std::string, ImVec4> GetColorFromConnectionState(ConnectionState state)
{
    switch (state)
    {
        case Connected:
            return {"Connected", ImVec4(0.2f, 0.7f, 0.2f, 1.0f)};
        case Connecting:
            return {"Connecting", ImVec4(0.8, 0.7f, 0.1f, 1.0f)};
        case Disconnected:
            return {"Disconnected", ImVec4(0.9, 0.4f, 0.4f, 1.0f)};
        case Failed:
            return {"Disconnecting", ImVec4(0.9, 0.4f, 0.4f, 1.0f)};
        case Closed:
            return {"Closed", ImVec4(0.9, 0.4f, 0.4f, 1.0f)};
        default:
            return {"Unknown", ImVec4(0.3f, 0.3f, 0.3f, 1.0f)};
    }
}

bool FrameWidget(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags, const ImVec4& color)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = ImGui::GetColorU32(color);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("[", "]");
    ImGui::RenderTextClipped(ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y), ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

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

std::string timeAgoString(const std::chrono::system_clock::time_point& lastSeenTime) {
    auto currentTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSeenTime);

    if (duration.count() < 60) {
        return "just now";
    } else if (duration.count() < 3600) {
        int minutes = duration.count() / 60;
        return (minutes == 1) ? "a minute ago" : (std::to_string(minutes) + " minutes ago");
    } else if (duration.count() < 86400) {
        int hours = duration.count() / 3600;
        return (hours == 1) ? "an hour ago" : (std::to_string(hours) + " hours ago");
    } else {
        int days = duration.count() / 86400;
        return (days == 1) ? "a day ago" : (std::to_string(days) + " days ago");
    }
}

std::chrono::system_clock::time_point restoreTimePointFromMilliseconds(std::uint64_t millisecondsSinceEpoch) {
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(millisecondsSinceEpoch));
}

ChatPanel::ChatPanel(IChatController& controller_, EventBus& eventBus) : controller(controller_), eventBus(eventBus)
{
    eventBus.Subscribe("SetSelectedChannelEvent", [this](const EventData& e) {
        auto eventData = static_cast<const SetSelectedChannelEvent&>(e);
        this->selectedChannelUuid = eventData.channelId;
    });
}

void ChatPanel::Update()
{
    ChatViewModel vm = this->controller.GetViewModel(this->selectedChannelUuid);
    ImVec4 peerNameColor = ImVec4(vm.peerData.profile.nameColor[0], vm.peerData.profile.nameColor[1], vm.peerData.profile.nameColor[2], 1.0f);

    ImGui::SameLine();
    //    ImGui::BeginDisabled(vm.chatHistory == nullptr);

    if (ImGui::BeginChild("PeerChatWindow", ImVec2(0,0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar))
    {

        if (ImGui::BeginMenuBar())
        {
            ImGui::PushStyleColor(0, peerNameColor);//ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
            ImGui::Text("%s", vm.peerData.username.c_str());
            ImGui::PopStyleColor();
            ImGui::Separator();

            auto [connectionStateText, connectionStateColor] = GetColorFromConnectionState(vm.peerData.connectionState);
            FrameWidget(connectionStateText.c_str(), ImVec2(0,0), ImGuiButtonFlags_None, connectionStateColor);


            ImGui::Separator();

            if (vm.peerData.connectionState != Connected) ImGui::BeginDisabled();

            if (!vm.isPeerContact)
            {
                if (ImGui::SmallButton("Add"))
                {
                    this->controller.AddPeerAsContact(vm.peerData.username);
                }
            }
            else
            {
                if (ImGui::SmallButton("Remove"))
                {
                    this->controller.RemovePeerFromContacts(vm.peerPublicKey);
                }
            }

            ImGui::SmallButton("Voice");
            ImGui::SmallButton("Video");

            ImGui::Separator();
            if (ImGui::SmallButton("Add"))
            {

            }


            if (vm.peerData.connectionState != Connected) ImGui::EndDisabled();

            // if (ImGui::BeginTable("table_columns_flags_checkboxes", 3, ImGuiTableFlags_None))
            // {
            //     // PushStyleCompact();
            //     for (int column = 0; column < 3; column++)
            //     {
            //         ImGui::TableNextColumn();
            //         ImGui::PushID(column);
            //         ImGui::AlignTextToFramePadding(); // FIXME-TABLE: Workaround for wrong text baseline propagation across columns
            //         // stretch the button to the column width (without this, the button would be square-shaped)
            //         ImGuiContext& g = *GImGui;
            //         float backup_padding_y = g.Style.FramePadding.y;
            //         g.Style.FramePadding.y = 0.0f;
            //         bool pressed = ImGui::ButtonEx("Stretch", ImVec2(-FLT_MIN, 0), ImGuiButtonFlags_AlignTextBaseLine);
            //         g.Style.FramePadding.y = backup_padding_y;
            //         ImGui::PopID();
            //     }
            //     // PopStyleCompact();
            //     ImGui::EndTable();
            // }
        }
        ImGui::EndMenuBar();

        if (ImGui::BeginChild("Chat", ImVec2(ImGui::GetContentRegionAvail().x - 150, 0), true, ImGuiWindowFlags_AlwaysAutoResize))
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
                            const ChatMessageInfo& message = chatHistory->at(i);
                            std::string timestamp = formatMilliseconds(message.timestamp);
                            ImVec4 nameColor = {message.senderProfile.nameColor[0], message.senderProfile.nameColor[1], message.senderProfile.nameColor[2], 1.0f};

                            ImGui::TableNextRow();
                            ImGui::TableNextRow();
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImColor(ImGui::GetStyle().Colors[paleBackground ? ImGuiCol_TableRowBg : ImGuiCol_TableRowBgAlt]));

                            ImGui::TableNextColumn();
                            ImGui::TextDisabled("%s", timestamp.c_str());

                            ImGui::TableNextColumn();
                            ImGui::PushStyleColor(0, nameColor);//ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
                            ImGui::Text("[ %s ]", message.senderProfile.displayName.c_str());
                            ImGui::PopStyleColor();

                            ImGui::TableNextRow();
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImColor(ImGui::GetStyle().Colors[paleBackground ? ImGuiCol_TableRowBg : ImGuiCol_TableRowBgAlt]));
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn();
                            ImGui::TextWrapped("%s", message.content.c_str());

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
            if (ImGui::InputText("Input", &this->inputBuf, input_text_flags))
            {
                util::trim(this->inputBuf);

                if (!this->inputBuf.empty())
                {
                    this->controller.SendMessage(this->selectedChannelUuid, this->inputBuf);
                }

                this->inputBuf.clear();

                reclaim_focus = true;
            }

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1);// Auto focus previous widget
        }
        ImGui::EndChild();

        ImGui::SameLine();


        if (ImGui::BeginChild("##peerInfo", ImVec2(145, 0), true,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar))
        {
            // ImGui::PushStyleColor(0, peerNameColor);//ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
            // ImGui::Text("%s", vm.peerData.profile.displayName.c_str());
            // ImGui::PopStyleColor();
            // ImGui::SameLine();
            // ImGui::TextWrapped("(%s)", vm.peerData.username.c_str());
            //
            // ImGui::SeparatorText("Profile");
            //
            // ImGui::BeginGroup();
            //
            // ImGui::TextDisabled("Description");
            // ImGui::TextWrapped("%s", vm.peerData.profile.description.empty() ? "-" : vm.peerData.profile.description.c_str());
            // ImGui::TextDisabled("Status");
            // ImGui::TextWrapped("%s", vm.peerData.profile.status.empty() ? "-" : vm.peerData.profile.status.c_str());
            // ImGui::TextDisabled("Client ID");
            //
            // std::string buf = vm.peerData.peerId;
            // ImGui::InputText("##clientidbox", &buf, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll, nullptr, nullptr);
            //
            // ImGui::TextDisabled("IP");
            // buf = vm.peerIpAddress;
            // ImGui::InputText("##ipbox", &buf, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll, nullptr, nullptr);
            // buf = vm.peerPublicKey;
            // ImGui::TextDisabled("PUBKEY");
            // ImGui::InputTextMultiline("##pubkeybox", &buf, ImVec2(0,0), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll, nullptr, nullptr);
            // ImGui::TextDisabled("Last seen");
            // if (vm.peerData.authenticated || vm.peerData.connectionState != Connected)
            // {
            //     ImGui::TextWrapped("%s", vm.peerData.connectionState == Connected ? "Now" : timeAgoString(restoreTimePointFromMilliseconds(vm.peerLastSeen)).c_str());
            // }
            // else
            // {
            //     ImGui::TextWrapped("%s", "Unknown");
            // }
            // ImGui::EndGroup();
            //
            // ImGui::SeparatorText("Actions");
            //
            // ImGui::BeginGroup();
            //
            // ImGui::EndGroup();

            ChannelDTO channelData = this->controller.GetChannelDTO(this->selectedChannelUuid);
            for (auto& member : channelData.members)
            {
                if (!member.peer)
                {
                    ImGui::Text("%s", member.fingerprint.c_str());
                }
                else
                {
                    ImGui::Text("%s", member.peer->profile.displayName.c_str());
                }
            }
        }
        ImGui::EndChild();
    }

    ImGui::EndChildFrame();

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
