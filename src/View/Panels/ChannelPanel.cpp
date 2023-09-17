#include "ChannelPanel.h"

ChannelPanel::ChannelPanel(IChannelController& controller_) : controller(controller_)
{
}

void ChannelPanel::Update()
{

    ChannelViewModel vm = this->controller.GetViewModel();

    ImGui::BeginGroup();

    if (ImGui::BeginChild("Npcs", ImVec2(200, -50), true,
                          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::Button("Add new user"))
        {
            this->addNewChatPrompt = true;
        }
        if (this->addNewChatPrompt)
        {
            ImGui::OpenPopup("New chat");
            if (ImGui::BeginPopupModal("New chat", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Enter a username");
                ImGui::Separator();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                bool enterPressed = ImGui::InputTextWithHint("##username", "username...", &this->usernameToConnectToBuf, ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::PopStyleVar();

                bool addNewChatPressed = ImGui::Button("Add new chat");

                if (enterPressed || addNewChatPressed)
                {
                    if (this->usernameToConnectToBuf[0] != '\0')
                    {
                        this->controller.AddNewChatPeer(this->usernameToConnectToBuf);
                        this->usernameToConnectToBuf.clear();
                        this->addNewChatPrompt = false;
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel"))
                {
                    this->addNewChatPrompt = false;
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
            bool isSelected = vm.selectedPeerId == peerId;

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
                this->controller.SetSelectedPeerId(peerId);
            }
            if (hasColor) ImGui::PopStyleColor();
        }
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
