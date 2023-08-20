#include "ChannelPanel.h"

#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <string>


ChannelPanel::ChannelPanel(IChannelController& controller_) : controller(controller_)
{
}

void ChannelPanel::Update()
{

    ChannelViewModel vm = controller.GetViewModel();

    ImGui::BeginGroup();

    ImGui::BeginChild("Npcs", ImVec2(200, -50), true,
                      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::Button("Add new user"))
    {
        addNewChatPrompt = true;
    }
    if (addNewChatPrompt)
    {
        ImGui::OpenPopup("New chat");
        if (ImGui::BeginPopupModal("New chat", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter a username");
            ImGui::Separator();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            bool enterPressed = ImGui::InputTextWithHint("##username", "otismus prime", &UsernameToConnectToBuf, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar();

            bool addNewChatPressed = ImGui::Button("Add new chat");

            if (enterPressed || addNewChatPressed)
            {
                if (this->UsernameToConnectToBuf[0] != '\0')
                {
                    controller.AddNewChatPeer(this->UsernameToConnectToBuf);
                    this->UsernameToConnectToBuf.clear();
                    addNewChatPrompt = false;
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                addNewChatPrompt = false;
            }

            ImGui::EndPopup();
        }
    }
    ImGui::Separator();

    // draw chat names

    //Controller code (controller needs to return list of connections from the model)
    for (const auto& peerConnection: vm.peerDataMap)
    {
        // TODO: what the hell is going on
        std::string peerId = peerConnection.first;
        bool isSelected = selectedChat == peerId;

        ConnectionState state = peerConnection.second.connectionState;

        std::string displayText = peerId;
        ImVec4 color;
        bool hasColor = false;
        switch (state)
        {
            case ConnectionState::New: {
                break;
            }
            case ConnectionState::Connecting: {
                hasColor = true;
                color = ImVec4(1.0f, 0.9f, 0.2f, 1.0f);
                displayText += " [Connecting...]";
                break;
            }
            case ConnectionState::Connected: {
                break;
            }
            case ConnectionState::Disconnected: {
                hasColor = true;
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                displayText += " [Disconnected]";
                break;
            }
            case ConnectionState::Failed: {
                hasColor = true;
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                displayText += " [Connection failed]";
                break;
            }
            case ConnectionState::Closed: {
                hasColor = true;
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                displayText += " [Connection closed]";
                break;
            }
            default: {
                hasColor = false;
                break;
            }
        }

        //        auto peerDataIt = historyMap.find(peerId);
        //        PeerData *pPeerData = nullptr;
        //        if (peerDataIt != historyMap.end())
        //        {
        //            pPeerData = &peerDataIt->second;
        //        }

        //        if (pPeerData != nullptr)
        //        {
        //            if (isSelected)
        //            {
        //                pPeerData->unreadMessageCount = 0;
        //            }
        //            if (pPeerData->unreadMessageCount > 0)
        //            {
        //                displayText += std::format(" [{} unread message{}]", pPeerData->unreadMessageCount,
        //                                           pPeerData->unreadMessageCount == 1 ? "" : "s");
        //            }
        //        }

        if (hasColor) ImGui::PushStyleColor(ImGuiCol_Text, color);
        if (ImGui::Selectable(displayText.c_str(), isSelected))
        {
            //            if (pPeerData != nullptr)
            //                console.SetLogHistory(pPeerData->history);
            selectedChat = peerId;
            controller.SetSelectedPeerId(peerId);
        }
        if (hasColor) ImGui::PopStyleColor();
    }

    ImGui::EndChild();
}

void ChannelPanel::Draw()
{
    Update();

    if (IsVisible())
    {
        // Draw the channels panel
    }
}

bool ChannelPanel::IsVisible()
{
    return false;
}
