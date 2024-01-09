//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


ChatViewModel ChatController::GetViewModel()
{
    const auto chatHistory = userLogic.GetSelectedChatHistory();

    const ChatHistory* chatHistoryPtr = nullptr;
    size_t chatHistorySize = 0;

    if (chatHistory)
    {
        chatHistoryPtr = chatHistory;
        chatHistorySize = chatHistory->size();
    }

    const UserDataManager& userDataManager = userLogic.GetUserDataManager();

    const std::string& currentPeerId = this->userLogic.GetSelectedPeerId();

    const auto peerData = userDataManager.GetPeerData(currentPeerId);

    // TODO: fix last seen time
    const std::string publicKey = userDataManager.GetPeerVerificationKey(currentPeerId);
    const uint64_t peerLastSeen = userDataManager.IsContactKeyAlreadyRegistered(publicKey) ?
                            userDataManager.GetContactFromPublicKey(publicKey).value_or(ContactData{}).lastSeen : 0;

    return {chatHistoryPtr,
            chatHistorySize,
            currentPeerId,
            publicKey,
            userDataManager.GetPeerIpAddress(currentPeerId),
            peerLastSeen,
            userDataManager.IsContactKeyAlreadyRegistered(publicKey),
            peerData.value_or(PeerData{})};
}

void ChatController::SendMessage(const std::string& message)
{
    this->userLogic.SendTextMessage(message);

    this->userLogic.AppendSelectedChatHistory(message);
}

void ChatController::AddPeerAsContact(const std::string& peerId)
{
    this->userLogic.AddPeerAsContact(peerId);
}

void ChatController::RemovePeerFromContacts(const std::string& peerId)
{
    this->userLogic.RemovePeerFromContacts(peerId);
}
