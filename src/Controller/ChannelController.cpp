//
// Created by Jean on 8/18/2023.
//

#include "ChannelController.h"
#include <Model/ApplicationLogic/Command/SetSelectedPeerIdCommand.h>

#include <memory>


ChannelViewModel ChannelController::GetViewModel()
{
    std::unordered_map<std::string, RemoteUserDTO> remoteUserDTOs;

    auto peerMap = this->userLogic.GetUserDataManager().GetRemoteUserMap();
    for (const auto& remoteUser : peerMap.GetAll())
    {
        RemoteUserDTO remoteUserDTO;
        remoteUserDTO.username = remoteUser->GetFingerprint(); // todo
        remoteUserDTO.fingerprint = remoteUser->GetFingerprint();
        remoteUserDTO.peerId = remoteUser->GetFingerprint(); // todo
        remoteUserDTO.profile = remoteUser->GetProfile();
        remoteUserDTO.unreadMessageCount = 0; // todo
        remoteUserDTO.connectionState = remoteUser->IsOnline() ? Connected : Closed;// todo peer->GetConnectionState();
        remoteUserDTO.authenticated = remoteUser->IsOnline();// todo peer->IsAuthenticated();

        remoteUserDTOs[remoteUser->GetFingerprint()] = std::move(remoteUserDTO);
    }

    std::unordered_map<std::string, ChannelDTO> channelDTOs;

    auto channelMap = this->userLogic.GetUserDataManager().GetChannelMap();
    for (const auto& channel : channelMap.GetChannels())
    {
        ChannelDTO channelDTO;
        channelDTO.uuid = channel->GetUuid();
        channelDTO.name = channel->GetName();
        channelDTO.localName = channel->GetName();
        channelDTO.description = channel->GetDescription();

        std::vector<ChannelMemberDTO> channelMemberDTOs;
        for (const auto& member: channel->GetMembers() | std::views::values)
        {
            ChannelMemberDTO channelMemberDTO;
            channelMemberDTO.fingerprint = member.GetFingerprint();
            channelMemberDTO.role = member.GetRole().name;

            channelMemberDTOs.push_back(std::move(channelMemberDTO));
        }

        channelDTO.members = std::move(channelMemberDTOs);

        channelDTOs[channel->GetUuid()] = std::move(channelDTO);
    }


    return {std::move(remoteUserDTOs),
            std::move(channelDTOs),
            this->userLogic.GetContactMap(),
            this->userLogic.GetSelectedPeerId(),
            this->userLogic.GetUserName()};
}

void ChannelController::AddNewChatPeer(const std::string& peerName_)
{
    // this->userLogic.AddNewChatPeer(peerName_, [this](std::weak_ptr<Peer> peer) {
        // auto peerPtr = peer.lock();
        // if (!peerPtr)
            // return;
        // this->userLogic.CreatePeerDMChannel(peerPtr);
    // });
    std::cout << "ChannelController::AddNewChatPeer" << std::endl;
    this->userLogic.AddRemoteUser(peerName_, peerName_);
}

void ChannelController::SetSelectedPeerId(const std::string& peerId_)
{
    auto remoteUser = this->userLogic.GetUserDataManager().GetRemoteUserMap().Get(peerId_);
    if (!remoteUser)
        return;

    this->userLogic.CreateUserDMChannel(remoteUser);
    // this->commandManager.ExecuteCommand(std::make_shared<SetSelectedPeerIdCommand>(
            // this->userLogic,
            // this->userLogic.GetSelectedPeerId(),
            // peerId_));
}

void ChannelController::RequestUserFingerprint(const std::string& fingerprint, std::function<void(nlohmann::json)> callback)
{
    this->userLogic.QueryUserFingerprint(fingerprint, [callback](nlohmann::json json) {
        callback(json);
    });
}
