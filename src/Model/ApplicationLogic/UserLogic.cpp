//
// Created by Jean on 8/29/2023.
//

#include "UserLogic.h"
#include <util/crypto/AESEncryption.h>
#include <util/crypto/Keygen.h>
#include <util/crypto/RSAEncryption.h>

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>


const ChatHistory* UserLogic::GetSelectedChatHistory() const
{
    if (!this->user.selectedChat.empty() && !this->user.peerMap.empty())
    {
        return &this->user.peerMap.at(this->user.selectedChat).chatHistory;
    }
    return nullptr;
}

void UserLogic::AppendSelectedChatHistory(const std::string& message)
{
    auto peer = this->user.peerMap.find(this->user.selectedChat);

    if (peer != this->user.peerMap.end())
    {
        peer->second.chatHistory.emplace_back(
                message,
                duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                this->user.username);
    }
}

void UserLogic::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessageInfo& chatMessage)
{
    auto peer = this->user.peerMap.find(peerId);

    if (peer != this->user.peerMap.end())
    {
        peer->second.chatHistory.emplace_back(chatMessage);
    }
}

void UserLogic::CreateNewChatHistory(const std::string& peerId_)
{
    //    this->user.peerMap.emplace(peerId_, Peer{});
}

void UserLogic::UpdatePeerState(const std::string& peerId, const ConnectionState& state)
{
    //    auto result = this->user.peerMap.insert({peerId, {}});
    auto peerIt = this->user.peerMap.find(peerId);
    if (peerIt != this->user.peerMap.end())
    {
        peerIt->second.peerData.connectionState = state;
    }
}

void UserLogic::IncrementPeerUnreadMessageCount(const std::string& peerId)
{
    ++(this->user.peerMap.at(peerId).peerData.unreadMessageCount);
}

void UserLogic::Reset(const std::string& username)
{
    this->user = User{username};
}

const std::string& UserLogic::GetUserName() const
{
    return this->user.username;
}

void UserLogic::SendTextMessage(const std::string& message)
{
    this->textChatAPI.SendMessageToPeer(this->user.selectedChat, message);
}

const std::unordered_map<std::string, PeerData> UserLogic::GetPeerDataMap() const
{
    std::unordered_map<std::string, PeerData> peerDataMap;

    for (auto& peer: this->user.peerMap)
    {
        peerDataMap.insert({peer.first, peer.second.peerData});
    }
    return peerDataMap;
}

void UserLogic::AddNewChatPeer(const std::string& peerId)
{
    Peer peer(peerId);
    this->RegisterPeer(peer);
    this->peeringAPI.OpenPeerConnection(peerId, [this, peerId] {
        std::cout << "Communications with " << peerId << "are READY.";
        this->peeringAPI.OnPeerMessage(peerId, [this, &peerId](auto& message) { this->HandlePeerMessage(peerId, std::forward<decltype((message))>(message)); });

        if (auto peer = this->GetPeer(peerId))
            peer->ongoingExchanges.StartNewExchange(std::make_shared<TextRequestExchange>(
                    TextRequestExchange::AwaitingResponse,
                    [] {}));
        this->peeringAPI.SendMessage(peerId, TextRequest{});
        CreateNewChatHistory(peerId);
    });
}

void UserLogic::SetSelectedPeerId(const std::string& peerId)
{
    this->user.selectedChat = peerId;
}

const size_t& UserLogic::GetMaxNameLength()
{
    return User::maxNameLength;
}

bool UserLogic::LoginWithNewUser(const std::string& username_, const std::string& password_)
{
    const auto& userInfos = this->localUsersAPI.GetLocalUserInfos();
    const auto& userInfoIt = std::find_if(userInfos.begin(), userInfos.end(), [&username_](const UserData& userInfo) { return userInfo.permanentUsername == username_; });
    if (userInfoIt == userInfos.end())
    {
        return false;
    }
    const auto& userInfo = *userInfoIt;

    std::string decryptedPrivateKey = DecryptAES(
            userInfo.encryptedPrivateIdentificationKey,
            DeriveKeyFromPassword(password_, username_, 256 / 8));

    if (!ValidateKeysRSA(decryptedPrivateKey, userInfo.publicIdentificationKey))
        return false;

    this->Reset(username_);
    this->connectionAPI.Disconnect();

    this->peeringAPI.OnPeerConnectionStateChange([this](const PeerConnectionStateChangeEvent& e) {
        this->UpdatePeerState(e.peerId, e.connectionState);
        if (e.connectionState == ConnectionState::Connected)
        {
            this->CreateNewChatHistory(e.peerId);
        }
    });
    this->textChatAPI.OnChatMessage([this](const ChatMessageInfo& e) {
        this->IncrementPeerUnreadMessageCount(e.senderId);
        this->AddChatMessageToPeerChatHistory(e.senderId, {e.content, e.timestamp, e.senderId});
    });

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "localhost";
    const std::string signalingServerPort = "8000";
    ConnectionConfig connectionConfig = {signalingServer, signalingServerPort};
    PeeringConfig peeringConfig = {stunServer, stunServerPort};
    this->connectionAPI.Init(connectionConfig);
    this->peeringAPI.Init(peeringConfig);

    this->connectionAPI.ConnectWithUsername(this->GetUserName());

    bool approveAllIncomingRequests = true;
    this->peeringAPI.OnPeerRequest([this, approveAllIncomingRequests](const std::string& peerId) {
        std::cout << "Peer request received from " << peerId << std::endl;

        if (approveAllIncomingRequests)
        {
            return true;
        }
        return false;
    });
    this->peeringAPI.OnNewPeer([this](const std::string& peerId) {
        Peer peer(peerId);
        this->RegisterPeer(peer);
        this->peeringAPI.OnPeerConnected(peerId, [this, peerId] {
            this->peeringAPI.OnPeerMessage(peerId, [this, peerId](BaseMessage<MessageType>& message) {
                this->HandlePeerMessage(peerId, message);
            });
        });
    });
    return true;
}

bool UserLogic::IsClientConnected() const
{
    return this->connectionAPI.IsConnected();
}

bool UserLogic::CreateNewUser(const std::string& username_, const std::string& password_)
{
    for (const auto& userInfo: this->localUsersAPI.GetLocalUserInfos())
    {
        if (userInfo.permanentUsername == username_)
        {
            return false;
        }
    }

    RSAKeyPair keypair = GenerateRSAKeyPair(512);

    std::string derivedEncryptionKey = DeriveKeyFromPassword(password_, username_, 256 / 8);
    std::string encryptedPrivateKey = EncryptAES(keypair.privateKey, derivedEncryptionKey);


    // Default profile
    UserProfile profile{};
    profile.displayName = username_;
    profile.description = "This is " + username_ + "'s description";
    profile.status = "This is " + username_ + "'s status";
    profile.nameColor = {0, 0, 0};

    UserData data{};
    data.permanentUsername = username_;
    data.encryptedPassword = EncryptAES(password_, derivedEncryptionKey);
    data.publicIdentificationKey = keypair.publicKey;
    data.encryptedPrivateIdentificationKey = encryptedPrivateKey;
    data.profile = profile;

    this->localUsersAPI.AddNewLocalUser(data);

    this->localUsersAPI.SaveLocalUserInfos();
    return true;
}

void UserLogic::LoadLocalUsers() const
{
    this->localUsersAPI.LoadLocalUserInfos();
}

const std::vector<UserData>& UserLogic::GetLocalUserInfos() const
{
    return this->localUsersAPI.GetLocalUserInfos();
}


const std::string& UserLogic::GetSelectedPeerId() const
{
    return this->user.selectedChat;
}

void UserLogic::HandlePeerMessage(const std::string& peerId, const BaseMessage<MessageType>& message)
{
    auto peer = this->GetPeer(peerId);
    if (!peer)
        return;

    switch (message.GetOpcode())
    {
        case ResetExchange::opcode: {
            ExchangeType exchangeType = static_cast<ExchangeType>(static_cast<const class ResetExchange&>(message).exchangeType);
            std::cout << "Exchange of type " << static_cast<int>(exchangeType) << " was reset by " << peer << std::endl;

            peer->ongoingExchanges.EndExchange(exchangeType);
            break;
        }
        case TextRequest::opcode: {

            if (peer->ongoingExchanges.StartNewExchange(std::make_shared<TextRequestExchange>(TextRequestExchange::AwaitingAck, [] {})))
            {
                this->peeringAPI.SendMessage(peerId, TextRequestResponse{});
            }
            break;
        }
        case TextRequestResponse::opcode: {

            if (peer->ongoingExchanges.SetExchangeState(ExchangeType::TextRequestExchange, TextRequestExchange::State::Completed))
            {
                this->peeringAPI.SendMessage(peerId, TextResponseAck{});
                this->textChatAPI.InitiateTextChat(peerId);
                peer->ongoingExchanges.EndExchange(ExchangeType::TextRequestExchange);
            }
            break;
        }
        case TextResponseAck::opcode: {

            if (peer->ongoingExchanges.SetExchangeState(ExchangeType::TextRequestExchange, TextRequestExchange::State::Completed))
            {
                this->textChatAPI.InitiateTextChat(peerId);
                peer->ongoingExchanges.EndExchange(ExchangeType::TextRequestExchange);
            }
            break;
        }
    }
}

void UserLogic::RegisterPeer(Peer peer)
{
    this->user.peerMap.emplace(peer.username, std::move(peer));
}

Peer* UserLogic::GetPeer(std::string peerId)
{
    auto peerIt = this->user.peerMap.find(peerId);
    if (peerIt != this->user.peerMap.end())
    {
        return &peerIt->second;
    }
    return nullptr;
}
