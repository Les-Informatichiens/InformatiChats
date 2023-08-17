#include <string>
#include "ChannelPanel.h"
#include "imgui.h"
#include "rtc/peerconnection.hpp"

ChannelPanel::ChannelPanel(Chat &chatClient) : chatClient(chatClient) {}

void ChannelPanel::Update() {
    ImGui::BeginChild("Npcs", ImVec2(200, -50), true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar);

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

            char* UsernameToConnectBuf{};
            const auto maxNameLength{32};
            bool enterPressed = ImGui::InputTextWithHint("##username", "otismus prime", UsernameToConnectBuf, maxNameLength, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopStyleVar();

            bool addNewChatPressed = ImGui::Button("Add new chat");

            if ( enterPressed || addNewChatPressed )
            {
                if(UsernameToConnectBuf[0] != '\0')
                {
                    chatClient.AttemptToConnectToPeer(UsernameToConnectBuf);
                    memset(UsernameToConnectBuf, 0, maxNameLength*sizeof(char));
                    addNewChatPrompt = false;
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel")) {
                addNewChatPrompt = false;
            }


            ImGui::EndPopup();
        }
    }
    ImGui::Separator();

    // draw chat names
    for (const auto& peerConnection : chatClient.GetPeerConnections())
    {
        const std::string& peerId = peerConnection.first;
        bool isSelected = selectedChat == peerId;

        rtc::PeerConnection::State state = peerConnection.second->state();

        std::string displayText = peerId;
        ImVec4 color;
        bool hasColor = false;
        switch (state) {
            case rtc::PeerConnection::State::New:
                break;
            case rtc::PeerConnection::State::Connecting: {
                hasColor = true;
                color = ImVec4(1.0f, 0.9f, 0.2f, 1.0f);
                displayText += " [Connecting...]";
                break;
            }
            case rtc::PeerConnection::State::Connected:
                break;
            case rtc::PeerConnection::State::Disconnected:
            {
                hasColor = true;
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                displayText += " [Disconnected]";
                break;
            }
            case rtc::PeerConnection::State::Failed: {
                hasColor = true;
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                displayText += " [Connection failed]";
                break;
            }
            case rtc::PeerConnection::State::Closed: {
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

        auto peerDataIt = historyMap.find(peerId);
        PeerData* pPeerData = nullptr;
        if (peerDataIt != historyMap.end())
        {
            pPeerData = &peerDataIt->second;
        }

        if (pPeerData != nullptr)
        {
            if (isSelected)
            {
                pPeerData->unreadMessageCount = 0;
            }
            if (pPeerData->unreadMessageCount > 0)
            {
                displayText += std::format(" [{} unread message{}]", pPeerData->unreadMessageCount, pPeerData->unreadMessageCount == 1 ? "" : "s");
            }
        }

        if (hasColor) ImGui::PushStyleColor(ImGuiCol_Text, color);
        if (ImGui::Selectable(displayText.c_str(), isSelected))
        {
            if (pPeerData != nullptr)
            //    console.SetLogHistory(pPeerData->history);
            selectedChat = peerId;
        }
        if (hasColor) ImGui::PopStyleColor();

    }

    ImGui::EndChild();
}

void ChannelPanel::Draw() {
    Update();

    if (IsVisible()) {
        // Draw the channels panel
    }
}

bool ChannelPanel::IsVisible() {
    return false;
}
