//
// Created by Jean on 8/29/2023.
//

#include "UserLogic.h"
#include <util/crypto/AESEncryption.h>
#include <util/crypto/Keygen.h>
#include <util/crypto/RSAEncryption.h>

#include <fstream>
#include <nlohmann/json.hpp>


const ChatHistory* UserLogic::GetSelectedChatHistory() const
{
    if (!this->user.selectedChat.empty() && !this->user.chatHistories.empty())
    {
        return &this->user.chatHistories.at(this->user.selectedChat);
    }
    return nullptr;
}

void UserLogic::AppendSelectedChatHistory(const std::string& message)
{
    auto chatHistory = this->user.chatHistories.find(this->user.selectedChat);

    if (chatHistory != this->user.chatHistories.end())
    {
        chatHistory->second.emplace_back(
                message,
                duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                this->user.username);
    }
}

void UserLogic::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage)
{
    auto chatHistory = this->user.chatHistories.find(peerId);
    if (chatHistory != this->user.chatHistories.end())
        chatHistory->second.emplace_back(chatMessage);
}

void UserLogic::CreateNewChatHistory(const std::string& peerId_)
{
    this->user.chatHistories.emplace(peerId_, ChatHistory{});
}

void UserLogic::UpdatePeerState(const std::string& peerId, const ConnectionState& state)
{
    auto result = this->user.peerDataMap.insert({peerId, {}});
    if (state == ConnectionState::Failed || state == ConnectionState::Closed || state == ConnectionState::Disconnected)
    {
        this->user.peerDataMap.erase(result.first->first);
        return;
    }
    result.first->second.connectionState = state;
}

void UserLogic::IncrementPeerUnreadMessageCount(const std::string& peerId)
{
    ++(this->user.peerDataMap.at(peerId).unreadMessageCount);
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
    this->chatAPI.SendMessageToPeer(this->user.selectedChat, message);
}

const std::unordered_map<std::string, PeerData>& UserLogic::GetPeerDataMap() const
{
    return this->user.peerDataMap;
}

void UserLogic::AddNewChatPeer(const std::string& peerId)
{
    this->chatAPI.AttemptToConnectToPeer(peerId);
    CreateNewChatHistory(peerId);
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
    this->chatAPI.Reset();

    this->chatAPI.SetOnPeerConnectionStateChange([this](const PeerConnectionStateChangeEvent& e) {
        this->UpdatePeerState(e.peerId, e.connectionState);
        if (e.connectionState == ConnectionState::Connected)
        {
            this->CreateNewChatHistory(e.peerId);
        }
    });
    this->chatAPI.SetOnMessageReceived([this](const ChatMessage& e) {
        this->IncrementPeerUnreadMessageCount(e.senderId);
        this->AddChatMessageToPeerChatHistory(e.senderId, {e.content,
                                                           duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                                                           e.senderId});
    });

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "informatichiens.com";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = {stunServer, stunServerPort, signalingServer, signalingServerPort};
    this->chatAPI.Init(config);

    this->chatAPI.AttemptConnectionWithUsername(this->GetUserName());

    return true;
}

bool UserLogic::IsClientConnected() const
{
    return this->chatAPI.IsConnected();
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

const std::unordered_map<std::string, std::string>& UserLogic::GetConfig() const
{
    return this->configAPI.GetConfig();
}
