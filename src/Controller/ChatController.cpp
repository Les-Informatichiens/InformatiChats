//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


#include "DTO/ChannelDTO.h"


ChatViewModel ChatController::GetViewModel(const std::string& selectedChannel)
{
    const UserDataManager& userDataManager = userLogic.GetUserDataManager();

    auto channel = userDataManager.GetChannelMap().GetChannel(selectedChannel);
    if (!channel)
        return {};

    const auto& chatHistory = channel->GetChatHistory();//userLogic.GetSelectedChatHistory();

    const ChatHistory* chatHistoryPtr = nullptr;
    size_t chatHistorySize = 0;

    chatHistoryPtr = &chatHistory;
    chatHistorySize = chatHistory.size();


    const std::string& currentPeerId = this->userLogic.GetSelectedPeerId();

    auto& peerMap = userDataManager.GetPeerMap_();
    auto peer = peerMap.TryGetPeer(currentPeerId);
    RemoteUserDTO peerDTO {};

    uint64_t peerLastSeen = 0;
    std::string publicKey;
    std::string ipAddress;
    if (peer)
    {
        peerDTO.username = peer->GetUsername();
        peerDTO.peerId = peer->GetPeerId();
        peerDTO.fingerprint = peer->GetFingerprint();
        peerDTO.profile = peer->GetProfile();
        peerDTO.unreadMessageCount = peer->GetUnreadMessageCount();
        peerDTO.connectionState = peer->GetConnectionState();
        peerDTO.authenticated = peer->IsAuthenticated();

        publicKey = peer->GetVerfiedPublicKey();
        peerLastSeen = userDataManager.IsContactKeyAlreadyRegistered(publicKey) ?
                            userDataManager.GetContactFromPublicKey(publicKey).value_or(ContactData{}).lastSeen : 0;
        ipAddress = peer->GetIpAddress();
    }

    return {chatHistoryPtr,
            chatHistorySize,
            publicKey,
            ipAddress,
            peerLastSeen,
            userDataManager.IsContactKeyAlreadyRegistered(publicKey),
            peerDTO};
}

void ChatController::SendMessage(const std::string& destinationChannel, const std::string& message)
{
    this->userLogic.SendTextMessage(destinationChannel, message);

    // this->userLogic.AppendSelectedChatHistory(message);
}

void ChatController::AddPeerAsContact(const std::string& peerId)
{
    this->userLogic.AddPeerAsContact(peerId);
}

void ChatController::RemovePeerFromContacts(const std::string& peerId)
{
    this->userLogic.RemovePeerFromContacts(peerId);
}

void ChatController::AddUserToChannel(const std::string& fingerprint, const std::string& channelUuid)
{
    const UserDataManager& userDataManager = userLogic.GetUserDataManager();

    auto channel = userDataManager.GetChannelMap().GetChannel(channelUuid);
    if (!channel)
        return;

    auto remoteUser = userDataManager.GetRemoteUserMap().Get(fingerprint);
    if (!remoteUser)
        return;

    std::string role = channel->;
    this->userLogic.AddRemoteUserToChannel(channelUuid, remoteUser, );


}

ChannelDTO ChatController::GetChannelDTO(const std::string& channelUuid)
{
    const UserDataManager& userDataManager = userLogic.GetUserDataManager();

    auto channel = userDataManager.GetChannelMap().GetChannel(channelUuid);
    ChannelDTO channelDTO;
    if (channel)
    {
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
            if (auto remoteUser = member.GetRemoteUser().lock(); remoteUser)
            {
                channelMemberDTO.peer = std::make_optional(RemoteUserDTO{
                    .peerId = remoteUser->GetFingerprint(),
                    .fingerprint = remoteUser->GetFingerprint(),
                    .username = remoteUser->GetPermanentUsername(),
                    .profile = remoteUser->GetProfile(),
                    .unreadMessageCount = 0, // todo
                    .connectionState = remoteUser->IsOnline() ? Connected : Closed, // todo
                    .authenticated = remoteUser->IsOnline(), // todo
                });
            }
            else
            {
                channelMemberDTO.peer = std::nullopt;
            }

            channelMemberDTOs.push_back(std::move(channelMemberDTO));
        }
        channelDTO.members = std::move(channelMemberDTOs);
    }
    return channelDTO;
}
