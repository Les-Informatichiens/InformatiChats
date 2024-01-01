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
    return this->userDataManager.GetSelectedChatHistory();
}

void UserLogic::AppendSelectedChatHistory(const std::string& message) const
{
    this->userDataManager.AppendSelectedChatHistory(message);
}

void UserLogic::AddSystemLogToPeerChatHistory(const std::string& peerId, const std::string& message, std::array<float, 3> color = {0.4, 1.0f, 0.4f}) const
{
    this->userDataManager.AddChatMessageToPeerChatHistory(peerId, {
        message,
        duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
        "System",
        {
            "System",
            "System",
            "System",
            color
        }
    });
}

void UserLogic::AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessageInfo& chatMessage) const
{
    this->userDataManager.AddChatMessageToPeerChatHistory(peerId, chatMessage);
}

void UserLogic::CreateNewChatHistory(const std::string& peerId_)
{
    //TODO: refactor this
    //this->user.peerMap.emplace(peerId_, Peer{});
}

void UserLogic::UpdatePeerState(const std::string& peerId, const ConnectionState& state)
{
    this->userDataManager.UpdatePeerState(peerId, state);

    if (const auto peerIp = this->peeringAPI.GetPeerIpAddress(peerId); peerIp.has_value())
    {
        this->userDataManager.UpdatePeerIpAddress(peerId, peerIp.value());
    }
}

void UserLogic::IncrementPeerUnreadMessageCount(const std::string& peerId)
{
    this->userDataManager.IncrementPeerUnreadMessageCount(peerId);
}

void UserLogic::VerifyPeerAuthentication(const std::string& peerId, const std::string& publicKey)
{
    auto peerExchanges = this->userDataManager.GetPeerExchanges(peerId);
    if (!peerExchanges)
        return;


    auto [hasStarted, exchangePtr] = peerExchanges->StartNewExchange<OfferAuthChallengeRequest>([this, peerId, publicKey](bool success) {
        if (!success)
        {
            this->peeringAPI.SendMessage(peerId, ResetExchange{ExchangeType::AuthChallengeExchange});
        }
        else
        {
            std::cout << "Exchange of type " << static_cast<int>(ExchangeType::AuthChallengeExchange) << " with " << peerId << " was successful" << std::endl;
        }
    });

    std::string originalToken = "banana";
    std::string encryptedToken = EncryptRSA(originalToken, publicKey);

    if (hasStarted)
    {
        exchangePtr->addObserver<OfferAuthChallengeRequest::CompletedState>([this, originalToken, peerId, publicKey](const std::shared_ptr<ExchangeState>& newState) {
            auto decryptedToken = newState->getUserData<std::string>();
            if (decryptedToken == originalToken)
            {
                this->userDataManager.SetPeerVerificationKey(peerId, publicKey);
                this->userDataManager.UpdatePeerVerificationStatus(peerId, true);
                this->AddSystemLogToPeerChatHistory(peerId, "Authentication successful.", {0.4f, 1.0f, 0.4f});
                std::cout << "Authentication successful!" << std::endl;
            }
            else
            {
                this->userDataManager.UpdatePeerVerificationStatus(peerId, false);
                this->AddSystemLogToPeerChatHistory(peerId, "Authentication failed.", {1.0f, 0.4f, 0.4f});
                std::cout << "Authentication failed!" << std::endl;
            }
        });
    }
    this->peeringAPI.SendMessage(peerId, AuthChallengeRequest{encryptedToken});
}

void UserLogic::AddPeerAsContact(const std::string& peerId)
{
    const auto peerExchanges = this->userDataManager.GetPeerExchanges(peerId);
    if (!peerExchanges)
        return;


    auto [hasStarted, exchangePtr] = peerExchanges->StartNewExchange<OfferContactRequest>();

    if (hasStarted)
    {
        exchangePtr->addObserver<OfferContactRequest::CompletedState>([this, peerId](const std::shared_ptr<ExchangeState>& newState) {
            const auto requestResponse = newState->getUserData<ContactRequestResponse>();
            if (requestResponse.accepted)
            {
                // this->userDataManager.AddPeerAsContact(peerId);
                ContactData contact;
                contact.permanentUsername = peerId;
                contact.publicIdentificationKey = requestResponse.publicIdentityKey;
                contact.lastSeen = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();;

                contact.profile = this->userDataManager.GetPeerProfile(peerId);
                this->userDataManager.AddContact(contact);
                VerifyPeerAuthentication(peerId, contact.publicIdentificationKey);
                this->userDataManager.SetPeerVerificationKey(peerId, contact.publicIdentificationKey);
                this->AddSystemLogToPeerChatHistory(peerId, "Contact request accepted! You can identify " + peerId + "!");
                std::cout << "Contact request accepted!" << std::endl;
            }
            else
            {
                this->AddSystemLogToPeerChatHistory(peerId, "Contact request rejected.", {1.0f, 0.4f, 0.4f});
                std::cout << "Contact request rejected!" << std::endl;
            }
        });
    }
    this->peeringAPI.SendMessage(peerId, ContactRequest{this->userDataManager.GetPublicIdentificationKey()});
}

void UserLogic::RemovePeerFromContacts(const std::string& contactPublicKey)
{
    const auto contact = this->userDataManager.GetContactFromPublicKey(contactPublicKey).value_or(ContactData{});
    this->userDataManager.SetPeerVerificationKey(contact.permanentUsername, "");
    this->userDataManager.UpdatePeerVerificationStatus(contact.permanentUsername, false);
    this->userDataManager.RemoveContact(contactPublicKey);
    this->AddSystemLogToPeerChatHistory(contact.permanentUsername, "Contact removed.", {1.0f, 0.4f, 0.4f});
}

void UserLogic::Reset(const UserData& userData)
{
    this->userDataManager.Reset(userData);
}

const std::string& UserLogic::GetUserName() const
{
    return this->userDataManager.GetUsername();
}

void UserLogic::SendTextMessage(const std::string& message)
{
    this->textChatAPI.SendMessageToPeer(this->userDataManager.GetSelectedChat(), message);
}

std::unordered_map<std::string, PeerData> UserLogic::GetPeerDataMap() const
{
    return this->userDataManager.GetPeerDataMap();
}

void UserLogic::AddNewChatPeer(const std::string& peerId)
{
    this->userDataManager.AddNewPeer(peerId);
    this->peeringAPI.OpenPeerConnection(peerId, [this, peerId] {
        std::cout << "Communications with " << peerId << "are READY.";
        this->AddSystemLogToPeerChatHistory(peerId, "Connection with " + peerId + " established successfully.", {0.4f, 1.0f, 0.4f});
        this->peeringAPI.OnPeerMessage(peerId, [this, &peerId](auto& message) { this->HandlePeerMessage(peerId, std::forward<decltype((message))>(message)); });
        this->InterrogatePeerPublicKey(peerId, [this, peerId](const std::string& publicKey) {
            // if (this->userDataManager.GetPeerVerificationKey(peerId).empty())
            // {
            //     this->userDataManager.SetPeerVerificationKey(peerId, publicKey);
            // }
            if (const auto contactOpt = this->userDataManager.GetContactFromPublicKey(publicKey); contactOpt.has_value())
            {
                this->VerifyPeerAuthentication(peerId, contactOpt->publicIdentificationKey);
            }
        });
        this->peeringAPI.SendMessage(peerId, UserProfileRequest{});

        this->InitiateTextRequest(peerId);
    });
}

void UserLogic::SetSelectedPeerId(const std::string& peerId)
{
    this->userDataManager.SetSelectedChat(peerId);
    if (!this->userDataManager.GetPeerVerificationKey(peerId).empty())
        this->VerifyPeerAuthentication(peerId, this->userDataManager.GetPeerVerificationKey(peerId));

    this->peeringAPI.SendMessage(peerId, UserProfileRequest{});
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

    this->Reset(userInfo);
    this->connectionAPI.Disconnect();

    this->userDataManager.SetDecryptedPrivateKey(decryptedPrivateKey);

    this->peeringAPI.OnPeerConnectionStateChange([this](const PeerConnectionStateChangeEvent& e) {

        auto onClose = [this, e] {
            auto key = this->userDataManager.GetPeerVerificationKey(e.peerId);
            if (auto contactOpt = this->userDataManager.GetContactFromPublicKey(key); contactOpt.has_value())
            {
                auto contact = *contactOpt;
                contact.lastSeen = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                this->userDataManager.UpdateContact(key, contact);
                this->userDataManager.UpdateToLocalUsers();
            }
        };

        this->UpdatePeerState(e.peerId, e.connectionState);
        if (e.connectionState == ConnectionState::Connected)
        {
            this->CreateNewChatHistory(e.peerId);
        }
        if (e.connectionState == ConnectionState::Disconnected)
        {
            onClose();
            this->AddSystemLogToPeerChatHistory(e.peerId, e.peerId + " has disconnected.", {1.0f, 0.4f, 0.4f});
            this->userDataManager.SetPeerVerificationKey(e.peerId, "");
            this->userDataManager.UpdatePeerVerificationStatus(e.peerId, false);
        }
        if (e.connectionState == ConnectionState::Failed)
        {
            onClose();
            this->AddSystemLogToPeerChatHistory(e.peerId, e.peerId + " connection failed.", {1.0f, 0.4f, 0.4f});
            this->userDataManager.SetPeerVerificationKey(e.peerId, "");
            this->userDataManager.UpdatePeerVerificationStatus(e.peerId, false);
        }
        if (e.connectionState == ConnectionState::Closed)
        {
            onClose();
            this->AddSystemLogToPeerChatHistory(e.peerId, e.peerId + " connection closed.", {1.0f, 0.4f, 0.4f});
            this->userDataManager.SetPeerVerificationKey(e.peerId, "");
            this->userDataManager.UpdatePeerVerificationStatus(e.peerId, false);
        }
    });
    this->textChatAPI.OnChatMessage([this](const ChatMessageInfo& e) {
        this->IncrementPeerUnreadMessageCount(e.senderId);
        this->AddChatMessageToPeerChatHistory(e.senderId, {e.content, e.timestamp, e.senderId, this->userDataManager.GetPeerProfile(e.senderId)});
    });

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "informatichiens.com";
    const std::string signalingServerPort = "51337";
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
        this->userDataManager.AddNewPeer(peerId);
        this->peeringAPI.OnPeerConnected(peerId, [this, peerId] {
            this->peeringAPI.OnPeerMessage(peerId, [this, peerId](BaseMessage<MessageType>& message) {
                this->HandlePeerMessage(peerId, message);
            });
            this->InterrogatePeerPublicKey(peerId, [this, peerId](const std::string& publicKey) {
                if (const auto contactOpt = this->userDataManager.GetContactFromPublicKey(publicKey); contactOpt.has_value())
                {
                    this->VerifyPeerAuthentication(peerId, contactOpt->publicIdentificationKey);
                }
            });
            this->AddSystemLogToPeerChatHistory(peerId, peerId + " has connected.", {0.4f, 1.0f, 0.4f});
            this->peeringAPI.SendMessage(peerId, UserProfileRequest{});
        });
    });

    for (const auto& [permanentUsername, publicIdentificationKey, lastSeen, profile]: userInfo.contacts | std::views::values)
    {
        this->AddNewChatPeer(permanentUsername);
        this->userDataManager.SetPeerVerificationKey(permanentUsername, publicIdentificationKey);
        this->userDataManager.UpdatePeerProfile(permanentUsername, profile);
    }

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
    profile.description = "This user has nothing to say about themselves";
    profile.status = "This is " + username_ + "'s status";
    profile.nameColor = {1.0f, 1.0f, 1.0f};

    UserData data{};
    data.permanentUsername = username_;
    data.encryptedPassword = EncryptAES(password_, derivedEncryptionKey);
    data.publicIdentificationKey = keypair.publicKey;
    data.encryptedPrivateIdentificationKey = encryptedPrivateKey;
    data.profile = profile;
    data.contacts = {};

    this->localUsersAPI.AddNewLocalUser(data);

    this->localUsersAPI.SaveLocalUserInfos();
    return true;
}

void UserLogic::LoadLocalUsers() const
{
    this->localUsersAPI.LoadLocalUserInfos();
}

void UserLogic::UpdateUserProfile(const UserProfile& userProfile) const
{
    this->userDataManager.UpdateUserProfile(userProfile);
    for (auto [peerId, peer] : this->userDataManager.GetPeerDataMap())
    {
        this->peeringAPI.SendMessage(peerId, UserProfileRequestResponse{userProfile});
    }
}

const std::vector<UserData>& UserLogic::GetLocalUserInfos() const
{
    return this->localUsersAPI.GetLocalUserInfos();
}


const std::string& UserLogic::GetSelectedPeerId() const
{
    return this->userDataManager.GetSelectedChat();
}

void UserLogic::HandlePeerMessage(const std::string& peerId, const BaseMessage<MessageType>& message)
{
    const auto peerExchanges = this->userDataManager.GetPeerExchanges(peerId);
    if (!peerExchanges)
        return;

    switch (message.GetOpcode())
    {
        case ResetExchange::opcode: {
            auto exchangeType = static_cast<ExchangeType>(dynamic_cast<const class ResetExchange&>(message).exchangeType);
            std::cout << "Exchange of type " << static_cast<int>(exchangeType) << " was reset by " << peerId << std::endl;

            peerExchanges->ForceFail(exchangeType);
            break;
        }
        case TextRequest::opcode: {
            this->HandleTextRequest(peerId);
            break;
        }
        case TextRequestResponse::opcode: {

            if (peerExchanges->SetExchangeState(ExchangeType::TextRequestExchange, OfferTextRequestExchange::CompletedState::Make()))
            {
//                this->peeringAPI.SendMessage(peerId, TextRequestResponse{});
                this->peeringAPI.SendMessage(peerId, TextResponseAck{});
//                this->textChatAPI.InitiateTextChat(peerId);
//                peer->ongoingExchanges.EndExchange(ExchangeType::TextRequestExchange);
            }
//            else
//            {
//                peer->ongoingExchanges.EndExchange(ExchangeType::TextRequestExchange);
//                this->peeringAPI.SendMessage(peerId, ResetExchange{});
//            }
            break;
        }
        case TextResponseAck::opcode: {
            if (peerExchanges->SetExchangeState(ExchangeType::TextRequestExchange, AnswerTextRequestExchange::CompletedState::Make()))
            {
//                this->textChatAPI.InitiateTextChat(peerId);
//                peer->ongoingExchanges.EndExchange(ExchangeType::TextRequestExchange);
            }
//            else
//            {
//                peer->ongoingExchanges.EndExchange(ExchangeType::TextRequestExchange);
//                this->peeringAPI.SendMessage(peerId, ResetExchange{});
//            }
            break;
        }
        case AuthChallengeRequest::opcode: {
            const auto authChallengeRequest = dynamic_cast<const class AuthChallengeRequest&>(message);
            std::cout << "Auth challenge request received from " << peerId << std::endl;

            const std::string decryptedToken = DecryptRSA(
                    authChallengeRequest.encryptedToken, this->userDataManager.GetDecryptedPrivateKey());

            std::cout << "Decrypted token: " << decryptedToken << std::endl;

            this->peeringAPI.SendMessage(peerId, AuthChallengeAnswer{decryptedToken});
            break;
        }
        case AuthChallengeAnswer::opcode: {

            const auto authChallengeAnswer = dynamic_cast<const class AuthChallengeAnswer&>(message);
            std::cout << "Auth challenge answer received from " << peerId << std::endl;

            std::cout << "Decrypted token: " << authChallengeAnswer.decryptedToken << std::endl;

            peerExchanges->SetExchangeState(ExchangeType::AuthChallengeExchange, OfferAuthChallengeRequest::CompletedState::Make(authChallengeAnswer.decryptedToken));

            break;
        }
        case ContactRequest::opcode: {
            const auto contactRequest = dynamic_cast<const class ContactRequest&>(message);
            std::cout << "Contact request received from " << peerId << std::endl;

            // TODO: add some kind of UI prompt to accept or reject the request
            this->userDataManager.SetPeerVerificationKey(peerId, contactRequest.publicIdentityKey);
            uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->userDataManager.AddContact(ContactData{peerId, contactRequest.publicIdentityKey, timestamp, this->userDataManager.GetPeerProfile(peerId)});
            VerifyPeerAuthentication(peerId, contactRequest.publicIdentityKey);
            this->AddSystemLogToPeerChatHistory(peerId, "Contact request received! You can now identify " + peerId + "!");
            this->peeringAPI.SendMessage(peerId, ContactRequestResponse{true, this->userDataManager.GetPublicIdentificationKey()});
            break;
        }
        case ContactRequestResponse::opcode: {
            const auto contactRequestResponse = dynamic_cast<const class ContactRequestResponse&>(message);

            if (peerExchanges->SetExchangeState(ExchangeType::ContactRequestExchange, OfferContactRequest::CompletedState::Make(contactRequestResponse)))
            {
                std::cout << "Contact request response received from " << peerId << std::endl;
            }
            break;
        }
        case UserProfileRequest::opcode: {
            this->peeringAPI.SendMessage(peerId, UserProfileRequestResponse{this->userDataManager.GetUserProfile()});
            break;
        }
        case UserProfileRequestResponse::opcode: {
            const auto peerProfileResponse = dynamic_cast<const class UserProfileRequestResponse&>(message);
            this->userDataManager.UpdatePeerProfile(peerId, peerProfileResponse.userProfile);
            if (this->userDataManager.GetPeerData(peerId).value_or(PeerData{}).authenticated)
            {
                std::string peerPubKey = this->userDataManager.GetPeerVerificationKey(peerId);
                if (auto contactOpt = this->userDataManager.GetContactFromPublicKey(peerPubKey); contactOpt.has_value())
                {
                    auto contact = *contactOpt;
                    contact.profile = peerProfileResponse.userProfile;
                    this->userDataManager.UpdateContact(peerPubKey, contact);
                }
            }
            break;
        }
        case PublicKeyRequest::opcode: {
            this->peeringAPI.SendMessage(peerId, PublicKeyRequestResponse{this->userDataManager.GetPublicIdentificationKey()});
            break;
        }
        case PublicKeyRequestResponse::opcode: {
            const auto publicKeyResponse = dynamic_cast<const class PublicKeyRequestResponse&>(message);
            peerExchanges->SetExchangeState(ExchangeType::PublicKeyRequestExchange, OfferPublicKeyRequest::CompletedState::Make(publicKeyResponse.publicKey));
            break;
        }
    }
}

void UserLogic::InitiateTextRequest(const std::string& peerId)
{
    auto peerExchanges = this->userDataManager.GetPeerExchanges(peerId);
    if (!peerExchanges)
        return;

    auto exchange = [peerId, this](bool success){

        if (!success)
        {
            this->peeringAPI.SendMessage(peerId, ResetExchange{ExchangeType::TextRequestExchange});
        }
        else
        {
            std::cout << "Exchange of type " << static_cast<int>(ExchangeType::TextRequestExchange) << " with " << peerId << " was successful" << std::endl;
            this->textChatAPI.InitiateTextChat(peerId);
        }
    };

    peerExchanges->StartNewExchange<OfferTextRequestExchange>(std::move(exchange));

    this->peeringAPI.SendMessage(peerId, TextRequest{});
    CreateNewChatHistory(peerId);
}

void UserLogic::HandleTextRequest(const std::string& peerId)
{
    auto peerExchanges = this->userDataManager.GetPeerExchanges(peerId);
    if (!peerExchanges)
        return;

    auto exchange = [peerId, this](bool success) {
        if (!success)
        {
            this->peeringAPI.SendMessage(peerId, ResetExchange{ExchangeType::TextRequestExchange});
        }
        else
        {
            std::cout << "Exchange of type " << static_cast<int>(ExchangeType::TextRequestExchange) << " with " << peerId << " was successful" << std::endl;
            this->textChatAPI.InitiateTextChat(peerId);
        }
        // peerExchanges->EndExchange(ExchangeType::TextRequestExchange);
    };
    if (peerExchanges->StartNewExchange<AnswerTextRequestExchange>(std::move(exchange)).first)
    {
        this->peeringAPI.SendMessage(peerId, TextRequestResponse{});
    }
}

void UserLogic::InterrogatePeerPublicKey(const std::string& peerId, std::function<void(std::string)> onKeyReceived) const
{
    const auto peerExchanges = this->userDataManager.GetPeerExchanges(peerId);
    if (!peerExchanges)
        return;

    peerExchanges->EndExchange(ExchangeType::PublicKeyRequestExchange);
    if (auto [started, exchange] = peerExchanges->StartNewExchange<OfferPublicKeyRequest>(); started)
    {
        if (exchange)
        {
            exchange->addObserver<OfferPublicKeyRequest::CompletedState>([onKeyReceived](const std::shared_ptr<OfferPublicKeyRequest::CompletedState>& newState) {
                onKeyReceived(newState->getUserData<std::string>());
            });
            this->peeringAPI.SendMessage(peerId, PublicKeyRequest{});
        }
    }
}
