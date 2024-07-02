#include "ChannelPanel.h"


#include "View/Event/SetSelectedChannelEvent.h"


#include <imgui_internal.h>
#include <nlohmann/json.hpp>
#include <ranges>
#include <utility>

ChannelPanel::ChannelPanel(IChannelController& controller_, EventBus& eventBus) : controller(controller_), eventBus(eventBus)
{
}

void ChannelPanel::Update()
{

    ChannelViewModel vm = this->controller.GetViewModel();

    ImGui::BeginGroup();

    if (ImGui::BeginChild("Known users", ImVec2(200, (ImGui::GetContentRegionAvail().y - 50)/2), true,
                          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Contacts"))
            {
                for (const auto& contactData: vm.contactDataMap | std::views::values)
                {
                    std::string displayText = contactData.permanentUsername;
                    if (ImGui::Selectable(displayText.c_str()))
                    {
                    }
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Users"))
            {
                if (ImGui::InputText("##username", &this->fingerprintToQueryBuf, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    this->controller.RequestUserFingerprint(this->fingerprintToQueryBuf, [this](nlohmann::json response) {
                        this->userFingerprintResponse = std::move(response);
                    });
                }
                ImGui::Separator();
                if (!this->userFingerprintResponse.empty())
                {
                    ImGui::Text("Username: %s", this->userFingerprintResponse[0]["username"].get<std::string>().c_str());
                    ImGui::Text("Signaling ID: %s", this->userFingerprintResponse[0]["client_id"].get<std::string>().c_str());
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();

    if (ImGui::BeginChild("Peer connections", ImVec2(200, -50), true,
                          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::Button("Add new user"))
        {
            this->addNewChatPrompt = true;
        }
        if (this->addNewChatPrompt)
        {
            ImGui::OpenPopup("New chat");
            if (ImGui::BeginPopup("New chat", ImGuiWindowFlags_AlwaysAutoResize))
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

                if (ImGui::Button("Cancel") || ImGui::IsKeyPressedMap(ImGuiKey_Escape))
                {
                    this->addNewChatPrompt = false;
                }

                ImGui::EndPopup();
            }
        }
        ImGui::Separator();

        // draw chat names

        //Controller code (controller needs to return list of connections from the model)
        // for (const auto& peerConnection: vm.peerDataMap)
        // {
        //     // TODO: what the hell is going on
        //     std::string peerId = peerConnection.second.peerId;
        //     auto peer = peerConnection.second;
        //     bool isSelected = vm.selectedPeerId == peerId;
        //
        //     ConnectionState state = peerConnection.second.connectionState;
        //
        //     std::string displayText = peer.username.empty() ? "<unknown>" : peer.username;
        //     ImVec4 color;
        //     bool hasColor = false;
        //     switch (state)
        //     {
        //         case ConnectionState::New: {
        //             hasColor = false;
        //             displayText += " [New]";
        //             break;
        //         }
        //         case ConnectionState::Connecting: {
        //             hasColor = true;
        //             color = ImVec4(1.0f, 0.9f, 0.2f, 1.0f);
        //             displayText += " [Connecting...]";
        //             break;
        //         }
        //         case ConnectionState::Connected: {
        //             if (peerConnection.second.authenticated)
        //             {
        //                 hasColor = true;
        //                 color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
        //                 displayText += " [Authenticated]";
        //             }
        //             break;
        //         }
        //         case ConnectionState::Disconnected: {
        //             hasColor = true;
        //             color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        //             displayText += " [Disconnected]";
        //             break;
        //         }
        //         case ConnectionState::Failed: {
        //             hasColor = true;
        //             color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        //             displayText += " [Connection failed]";
        //             break;
        //         }
        //         case ConnectionState::Closed: {
        //             hasColor = true;
        //             color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        //             displayText += " [Connection closed]";
        //             break;
        //         }
        //         default: {
        //             hasColor = false;
        //             break;
        //         }
        //     }
        //
        //     //        auto peerDataIt = historyMap.find(peerId);
        //     //        PeerData *pPeerData = nullptr;
        //     //        if (peerDataIt != historyMap.end())
        //     //        {
        //     //            pPeerData = &peerDataIt->second;
        //     //        }
        //
        //     //        if (pPeerData != nullptr)
        //     //        {
        //     //            if (isSelected)
        //     //            {
        //     //                pPeerData->unreadMessageCount = 0;
        //     //            }
        //     //            if (pPeerData->unreadMessageCount > 0)
        //     //            {
        //     //                displayText += std::format(" [{} unread message{}]", pPeerData->unreadMessageCount,
        //     //                                           pPeerData->unreadMessageCount == 1 ? "" : "s");
        //     //            }
        //     //        }
        //
        //     if (hasColor) ImGui::PushStyleColor(ImGuiCol_Text, color);
        //     if (ImGui::Selectable(displayText.c_str(), isSelected))
        //     {
        //         this->controller.SetSelectedPeerId(peer.fingerprint);
        //     }
        //     if (hasColor) ImGui::PopStyleColor();
        // }

        for (const auto& channel : vm.channelDataMap | std::views::values)
        {
            ImGui::PushID(channel.uuid.c_str());
            std::string displayText = channel.name;
            bool isSelected = this->selectedChat == channel.uuid;

            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
            }
            if (ImGui::Selectable(displayText.c_str(), isSelected))
            {
                this->eventBus.Publish(SetSelectedChannelEvent{channel.uuid});
                this->selectedChat = channel.uuid;
                // this->controller.SetSelectedPeerId(channel.uuid);
            }
            if (isSelected)
            {
                ImGui::PopStyleColor();
            }
            ImGui::PopID();
        }
        ImGui::Separator();
        for (const auto& remoteUser : vm.peerDataMap | std::views::values)
        {
            std::string displayText = remoteUser.profile.displayName;
            if (displayText.empty())
            {
                displayText = remoteUser.fingerprint;
            }
            bool isSelected = this->selectedChat == remoteUser.fingerprint;

            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
            }
            if (ImGui::Selectable(displayText.c_str(), isSelected))
            {
                this->controller.SetSelectedPeerId(remoteUser.fingerprint);
            }
            if (ImGui::BeginItemTooltip())
            {
                ImVec4 peerNameColor = ImVec4(remoteUser.profile.nameColor[0], remoteUser.profile.nameColor[1], remoteUser.profile.nameColor[2], 1.0f);

                ImGui::PushStyleColor(0, peerNameColor);//ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
                ImGui::Text("%s", remoteUser.profile.displayName.c_str());
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::TextWrapped("(%s)", remoteUser.username.c_str());

                ImGui::SeparatorText("Profile");

                ImGui::BeginGroup();

                ImGui::TextDisabled("Description");
                ImGui::TextWrapped("%s", remoteUser.profile.description.empty() ? "-" : remoteUser.profile.description.c_str());
                ImGui::TextDisabled("Status");
                ImGui::TextWrapped("%s", remoteUser.profile.status.empty() ? "-" : remoteUser.profile.status.c_str());
                ImGui::TextDisabled("Client ID");

                std::string buf = remoteUser.peerId;
                ImGui::InputText("##clientidbox", &buf, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll, nullptr, nullptr);

                ImGui::TextDisabled("IP");
                // buf = vm.peerIpAddress;
                // ImGui::InputText("##ipbox", &buf, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll, nullptr, nullptr);
                // buf = vm.peerPublicKey;
                // ImGui::TextDisabled("PUBKEY");
                // ImGui::InputTextMultiline("##pubkeybox", &buf, ImVec2(0,0), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll, nullptr, nullptr);
                // ImGui::TextDisabled("Last seen");
                // if (vm.peerData.authenticated || vm.peerData.connectionState != Connected)
                // {
                    // ImGui::TextWrapped("%s", vm.peerData.connectionState == Connected ? "Now" : timeAgoString(restoreTimePointFromMilliseconds(vm.peerLastSeen)).c_str());
                // }
                // else
                // {
                    // ImGui::TextWrapped("%s", "Unknown");
                // }
                ImGui::EndGroup();

                // ImGui::SeparatorText("Actions");

                // ImGui::BeginGroup();

                // ImGui::EndGroup();
                ImGui::EndTooltip();
            }
            if (isSelected)
            {
                ImGui::PopStyleColor();
            }
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
