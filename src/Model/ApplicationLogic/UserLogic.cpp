//
// Created by Jean on 8/29/2023.
//

#include "UserLogic.h"


#include "Model/Models/RemoteUser.h"
#include "util/crypto/Hashing.h"
#include "util/crypto/UUID.h"

#include <util/crypto/AESEncryption.h>
#include <util/crypto/Keygen.h>
#include <util/crypto/RSAEncryption.h>


#include <memory>
#include <nlohmann/json.hpp>


// const ChatHistory* UserLogic::GetSelectedChatHistory() const
// {
//     return this->userDataManager.GetSelectedChatHistory();
// }
//
// void UserLogic::AppendSelectedChatHistory(const std::string& message) const
// {
//     this->userDataManager.AppendSelectedChatHistory(message);
// }

void UserLogic::AddSystemLogToChannelChatHistory(const std::string& channelUuid, const std::string& message, std::array<float, 3> color = {0.4, 1.0f, 0.4f}) const
{
    this->userDataManager.AddChatMessageToChannel(channelUuid, {
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

void UserLogic::AddChatMessageToChannelChatHistory(const std::string& channelUuid, const ChatMessageInfo& chatMessage) const
{
    this->userDataManager.AddChatMessageToChannel(channelUuid, chatMessage);
}

// void UserLogic::UpdatePeerState(const std::string& peerId, const ConnectionState& state)
// {
//     this->userDataManager.UpdatePeerState(peerId, state);
//
//     if (const auto peerIp = this->peeringAPI.GetPeerIpAddress(peerId); peerIp.has_value())
//     {
//         this->userDataManager.UpdatePeerIpAddress(peerId, peerIp.value());
//     }
// }

// void UserLogic::IncrementPeerUnreadMessageCount(const std::string& peerId)
// {
//     this->userDataManager.IncrementPeerUnreadMessageCount(peerId);
// }

void UserLogic::AddPeerAsContact(const std::string& peerId)
{
    auto peer = this->userDataManager.GetPeerMap_().TryGetPeer(peerId);
    if (!peer)
        return;

    auto& peerExchanges = peer->GetOngoingExchanges();


    auto [hasStarted, exchangePtr] = peerExchanges.StartNewExchange<OfferContactRequest>();

    if (hasStarted)
    {
        exchangePtr->addObserver<OfferContactRequest::CompletedState>([this, wpeer = std::weak_ptr(peer)](const std::shared_ptr<ExchangeState>& newState) {
            auto peer = wpeer.lock();
            if (!peer)
                return;

            const auto requestResponse = newState->getUserData<ContactRequestResponse>();
            if (requestResponse.accepted)
            {
                // this->userDataManager.AddPeerAsContact(peerId);
                ContactData contact;
                contact.permanentUsername = peer->GetUsername();
                // TODO use the already verified public key from the peer instead the one from the request
                contact.publicIdentificationKey = requestResponse.publicIdentityKey;
                contact.lastSeen = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();;

                contact.profile = peer->GetProfile();
                this->userDataManager.AddContact(contact);
                // VerifyPeerAuthentication(peerId, contact.publicIdentificationKey);
                // this->userDataManager.SetPeerVerificationKey(peerId, contact.publicIdentificationKey);
                // peer->AddSystemLogToPeerChatHistory("Contact request accepted! You can identify " + peer->GetPeerId() + "!");
                std::cout << "Contact request accepted!" << std::endl;
            }
            else
            {
                // peer->AddSystemLogToPeerChatHistory("Contact request rejected.", {1.0f, 0.4f, 0.4f});
                std::cout << "Contact request rejected!" << std::endl;
            }
        });
    }
    this->peeringAPI.SendMessage(peerId, ContactRequest{this->userDataManager.GetPublicIdentificationKey()});
}

void UserLogic::RemovePeerFromContacts(const std::string& contactPublicKey)
{
    const auto contact = this->userDataManager.GetContactFromPublicKey(contactPublicKey).value_or(ContactData{});
    // this->userDataManager.SetPeerVerificationKey(contact.permanentUsername, "");
    // this->userDataManager.UpdatePeerVerificationStatus(contact.permanentUsername, false);
    this->userDataManager.RemoveContact(contactPublicKey);
    this->AddSystemLogToChannelChatHistory(contact.permanentUsername, "Contact removed.", {1.0f, 0.4f, 0.4f});
}

std::string UserLogic::CreateUserDMChannel(const std::shared_ptr<RemoteUser>& remoteUser)
{
    std::string channelUuid = UUID::generateUUID();
    ChannelConfig config;
    config.maxMembers = 2;
    const auto channel = std::make_shared<Channel>(channelUuid, remoteUser->GetFingerprint(), "DM Channel", "DM Channel", config);
    ChannelMap& channelMap = this->userDataManager.GetChannelMap();
    channelMap.AddChannel(channel);

    Role role = PredefinedRoles::DM();
    channel->InitNew(this->userDataManager.GetPublicIdentificationKey(), {role}, role.name);
    // channel->AddMember(nullptr, role.name, false);

    auto peer = remoteUser->GetActivePeer();
    if (!peer)
        return channel->GetUuid();
    // todo change to a better role system
    // channel->AddMember(remoteUser->GetActivePeer()->GetVerfiedPublicKey(), remoteUser->GetFingerprint(), role.name, false);

    this->AddRemoteUserToChannel(channelUuid, remoteUser, role.name);

    // auto [started, exchange] = remoteUser->GetActivePeer()->GetOngoingExchanges().StartNewExchange<OfferChannelRequest>();
    // if (started)
    // {
    //     exchange->addObserver<OfferChannelRequest::CompletedState>([this, wchannel = std::weak_ptr(channel), wpeer = std::weak_ptr(remoteUser), role](const std::shared_ptr<ExchangeState>& newState) {
    //         const auto& channelResponse = newState->getUserData<ChannelRequestResponse>();
    //
    //         auto channel = wchannel.lock();
    //         if (!channel)
    //             return;
    //
    //         auto remoteUser = wpeer.lock();
    //         if (!remoteUser)
    //             return;
    //
    //         if (channelResponse.accepted)
    //         {
    //             channel->AddMember(remoteUser, role.name, true);
    //             // channel->SetMemberActiveUser(remoteUser->GetFingerprint(), remoteUser);
    //             this->InitiateTextRequest(remoteUser->GetActivePeer());
    //             std::cout << "Channel request accepted!" << std::endl;
    //         }
    //         else
    //         {
    //             std::cout << "Channel request rejected!" << std::endl;
    //         }
    //     });
    // }
    // remoteUser->GetActivePeer()->SendMessage(channel->CreateChannelRequest(role.name));

    return channel->GetUuid();
}

void UserLogic::AddRemoteUserToChannel(const std::string& channelUuid, const std::shared_ptr<RemoteUser>& remoteUser, const std::string& role)
{
    auto channel = this->userDataManager.GetChannelMap().GetChannel(channelUuid);
    if (!channel)
        return;


    auto [started, exchange] = remoteUser->GetActivePeer()->GetOngoingExchanges().StartNewExchange<OfferChannelRequest>();
    if (started)
    {
        exchange->addObserver<OfferChannelRequest::CompletedState>([this, wchannel = std::weak_ptr(channel), wpeer = std::weak_ptr(remoteUser), role](const std::shared_ptr<ExchangeState>& newState) {
            const auto& channelResponse = newState->getUserData<ChannelRequestResponse>();

            auto channel = wchannel.lock();
            if (!channel)
                return;

            auto remoteUser = wpeer.lock();
            if (!remoteUser)
                return;

            if (channelResponse.accepted)
            {
                channel->AddMember(remoteUser, role, true);
                // channel->SetMemberActiveUser(remoteUser->GetFingerprint(), remoteUser);
                this->InitiateTextRequest(remoteUser->GetActivePeer());
                std::cout << "Channel request accepted!" << std::endl;
            }
            else
            {
                std::cout << "Channel request rejected!" << std::endl;
            }
        });
    }
    remoteUser->GetActivePeer()->SendMessage(channel->CreateChannelRequest(role));
}

void UserLogic::Reset(const LocalUserData& userData)
{
    this->userDataManager.Reset(userData);
}

const std::string& UserLogic::GetUserName() const
{
    return this->userDataManager.GetUsername();
}

void UserLogic::SendTextMessage(const std::string& destinationChannel, const std::string& message)
{

    auto channel = this->userDataManager.GetChannelMap().GetChannel(destinationChannel);
    if (!channel)
        return;

    channel->AddChatMessageToPeerChatHistory({
        message,
        duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
        this->userDataManager.GetUsername(),
        this->userDataManager.GetUserProfile()
    });

    for (const auto& member: channel->GetMembers() | std::views::values)
    {
        if (member.GetFingerprint() == this->userDataManager.GetPublicIdentificationKey())
            continue;

        auto remoteUser = member.GetRemoteUser().lock();
        if (!remoteUser)
            continue;
        if (!remoteUser->IsOnline())
            continue;

        auto peer = remoteUser->GetActivePeer();

        // this->CreateUserDMChannel(remoteUser);

        this->textChatAPI.SendMessageToPeer(peer->GetPeerId(), destinationChannel, message);
    }
}

// const std::unordered_map<std::string, Peer>& UserLogic::GetPeerMap() const
// {
//     return this->userDataManager.GetPeerMap();
// }

const std::unordered_map<std::string, ContactData>& UserLogic::GetContactMap() const
{
    return this->userDataManager.GetContacts();
}


// void UserLogic::AddNewChatPeer(const std::string& signalingId, const std::function<void(std::weak_ptr<Peer>)>& callback)
// {
//     auto peer = std::make_shared<Peer>(this->peeringAPI);
//
//     std::string tempUUID = UUID::generateUUID();
//     this->userDataManager.GetPeerMap_().AddUnprocessedPeer(tempUUID, peer);
//
//     peer->Connect(signalingId);
//     // peer->OnConnected([this, wpeer = std::weak_ptr(peer), tempUUID, callback] {
//     //     auto peer = wpeer.lock();
//     //     if (!peer)
//     //         return;
//     //
//     //     auto peerId = peer->GetPeerId();
//     //     std::cout << "Communications with " << peerId << "are READY.";
//     //     peer->AddSystemLogToPeerChatHistory("Connection with " + peerId + " established successfully.", {0.4f, 1.0f, 0.4f});
//     //
//     //     peer->SubscribeEventGeneric([this, wpeer = std::weak_ptr(peer)](const BaseMessage<MessageType>& message) {
//     //         auto peer = wpeer.lock();
//     //         if (!peer)
//     //             return;
//     //
//     //         this->HandlePeerMessage(wpeer, message);
//     //     });
//     //
//     //     peer->RequestPublicKey([this, wpeer = std::weak_ptr(wpeer), tempUUID, callback](const std::string& publicKey) {
//     //         auto peer = wpeer.lock();
//     //         if (!peer)
//     //             return;
//     //         // if (const auto contactOpt = this->userDataManager.GetContactFromPublicKey(publicKey); contactOpt.has_value())
//     //         // {
//     //         peer->VerifyAuthenticity(publicKey, [this, wpeer = std::weak_ptr(wpeer), tempUUID, callback](bool success) {
//     //             auto peer = wpeer.lock();
//     //             if (!peer)
//     //                 return;
//     //             if (success)
//     //             {
//     //                 peer->SetFingerprint(Hashing::GeneratePublicKeyFingerprint(peer->GetVerfiedPublicKey()));
//     //                 this->userDataManager.GetPeerMap_().AcceptUnprocessedPeer(tempUUID);
//     //                 callback(peer);
//     //             }
//     //         });
//     //         // }
//     //     });
//     //
//     //     peer->RequestProfile();
//     //     this->InitiateTextRequest(peer);
//     // });
//     // peer->OnDisconnected([this, wpeer = std::weak_ptr(peer)] {
//     //     auto peer = wpeer.lock();
//     //     if (!peer)
//     //         return;
//     //
//     //     auto key = peer->GetVerfiedPublicKey();
//     //     if (auto contactOpt = this->userDataManager.GetContactFromPublicKey(key); contactOpt.has_value())
//     //     {
//     //         auto contact = *contactOpt;
//     //         contact.lastSeen = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//     //         this->userDataManager.UpdateContact(key, contact);
//     //         this->userDataManager.UpdateToLocalUsers();
//     //     }
//     // });
//     // this->textChatAPI.OnChatMessage(peer->GetPeerId(), [this, wpeer = std::weak_ptr(peer)](const ChatMessageInfo& e) {
//     //     auto peer = wpeer.lock();
//     //     if (!peer)
//     //         return;
//     //
//     //     this->IncrementPeerUnreadMessageCount(peer->GetFingerprint());
//     //     this->AddChatMessageToPeerChatHistory(peer->GetFingerprint(), {e.content, e.timestamp, e.senderId, peer->GetProfile()});
//     // });
// }

std::shared_ptr<RemoteUser> UserLogic::AddRemoteUser(const std::string& username, const std::string& fingerprint)
{
    auto remoteUser = this->userDataManager.GetRemoteUserMap().Get(fingerprint);
    if (!remoteUser)
    {
        remoteUser = std::make_shared<RemoteUser>(username, fingerprint);
        this->userDataManager.GetRemoteUserMap().Add(remoteUser);
    }

    this->QueryUserFingerprint(fingerprint, [this, wremoteUser = std::weak_ptr(remoteUser)](const nlohmann::json& clients) {
        auto remoteUser = wremoteUser.lock();
        if (!remoteUser)
            return;

        if (clients.size() > 1)
        {
            std::cout << "Conflict detected. Two clients with the same key." << std::endl;
            // for now we assume that there is only one client with a given key
            // for (const auto& client: clients)
            // {
            //     const std::string& signalingId = client["client_id"].get<std::string>();
            //
            //     auto peer = std::make_shared<Peer>(this->peeringAPI);
            //     peer->Connect(signalingId);
            //     this->HandleNewPeer(peer);
            // }
        }
        else if (clients.empty())
        {
            std::cout << "No other clients with the same key." << std::endl;
        }
        else if (clients.size() == 1)
        {
            const auto& client = clients[0];
            const std::string& signalingId = client["client_id"].get<std::string>();

            auto peer = std::make_shared<Peer>(this->peeringAPI);
            peer->Connect(signalingId);
            this->HandleNewPeer(peer);
            // remoteUser->AddPotentialPeer(signalingId, peer);
            // remoteUser->ProcessPotentialPeers();
        }
    });

    // return this->QueryUserFingerprint(fingerprint).then([&](const std::string& clientId) {
    //     remoteUser->AddPotentialPeer(clientId, std::make_shared<Peer>(this->peeringAPI));
    //     remoteUser->ProcessPotentialPeers();
    //     // Resolve the promise with the remoteUser
    //     return promise::resolve(remoteUser);
    // },
    // [&] {
    //     std::cout << "No other clients with the same key." << std::endl;
    //
    //     // Reject the promise
    //     return promise::reject();
    // }).finally([&] {
    //     if (remoteUser)
    //     {
    //         // Only add to the map if the promise was resolved
    //     }
    // });
    return remoteUser;
}

void UserLogic::RegisterPeerRemoteUser(std::shared_ptr<Peer> peer, const std::string& fingerprint)
{
    auto& remoteUserMap = this->userDataManager.GetRemoteUserMap();
    std::shared_ptr<RemoteUser> remoteUser = nullptr;
    if (remoteUserMap.Contains(fingerprint))
    {
        remoteUser = remoteUserMap.Get(fingerprint);
    }
    else
    {
        remoteUser = std::make_shared<RemoteUser>(peer->GetUsername(), fingerprint);
        remoteUserMap.Add(remoteUser);
    }
    remoteUser->SetActivePeer(peer);

    peer->SubscribeEventGeneric([this, wremoteUser = std::weak_ptr(remoteUser)](const BaseMessage<MessageType>& message) {
        auto remoteUser = wremoteUser.lock();
        if (!remoteUser)
            return;

        this->HandleRemoteUserMessage(remoteUser, remoteUser->GetActivePeer(), message);
    });
    remoteUser->RefreshProfile();
    this->textChatAPI.OnChatMessage(remoteUser->GetActivePeer()->GetPeerId(), [this, wremoteUser = std::weak_ptr(remoteUser)](const ChatMessage& m) {
        auto remoteUser = wremoteUser.lock();
        if (!remoteUser || !remoteUser->IsOnline())
            return;

        auto channel = this->userDataManager.GetChannelMap().GetChannel(m.destinationChannel);
        if (!channel)
            return;
        const auto message = ChatMessageInfo{std::move(m.content), std::chrono::milliseconds(m.timestamp), remoteUser->GetFingerprint(), remoteUser->GetProfile()};
        channel->AddChatMessageToPeerChatHistory(std::move(message));
    });
}

void UserLogic::QueryUserFingerprint(const std::string& fingerprint, std::function<void(nlohmann::json)> callback)
{
    this->connectionAPI.QueryClientsByPublicKey(fingerprint, [this, callback](const nlohmann::json& clients) {
        if (clients.size() > 1)
        {
            std::cout << "Conflict detected. Two clients with the same key." << std::endl;
        }
        else if (clients.empty())
        {
            std::cout << "No other clients with the same key." << std::endl;
        }
        else if (clients.size() == 1)
        {
            callback(clients);
        }
    });
}
// promise::Promise UserLogic::QueryUserFingerprint(const std::string& fingerprint)
// {
//     return promise::newPromise([this, fingerprint](promise::Defer& defer) {
//         this->connectionAPI.QueryClientsByPublicKey(fingerprint, [this, defer](const nlohmann::json& clients) {
//             if (clients.size() > 1)
//             {
//                 std::cout << "Conflict detected. Two clients with the same key." << std::endl;
//             }
//             else if (clients.empty())
//             {
//                 std::cout << "No other clients with the same key." << std::endl;
//                 defer.reject();
//             }
//             else if (clients.size() == 1)
//             {
//                 defer.resolve(clients[0]["client_id"].get<std::string>());
//             }
//         });
//     });
// }

void UserLogic::SetSelectedPeerId(const std::string& peerId)
{
    auto peer = this->userDataManager.GetPeerMap_().TryGetPeer(peerId);
    if (!peer)
        return;

    this->userDataManager.SetSelectedChat(peerId);

    peer->SendMessage(UserProfileRequest{});
}

const size_t& UserLogic::GetMaxNameLength()
{
    return LocalUser::maxNameLength;
}

bool UserLogic::LoginWithNewUser(const std::string& username_, const std::string& password_)
{
    const auto& userInfos = this->localUsersAPI.GetLocalUserInfos();
    const auto& userInfoIt = std::find_if(userInfos.begin(), userInfos.end(), [&username_](const LocalUserData& userInfo) { return userInfo.permanentUsername == username_; });
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

    // re-init the new chat client
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "localhost";
    const std::string signalingServerPort = "8000";
    ConnectionConfig connectionConfig = {signalingServer, signalingServerPort};
    PeeringConfig peeringConfig = {Hashing::GeneratePublicKeyFingerprint(userInfo.publicIdentificationKey), stunServer, stunServerPort};
    this->connectionAPI.Init(connectionConfig);
    this->peeringAPI.Init(peeringConfig);

    this->connectionAPI.ConnectWithUsername(this->GetUserName(), userInfo.publicIdentificationKey);

    bool approveAllIncomingRequests = true;
    this->peeringAPI.OnPeerRequest([this, approveAllIncomingRequests](const std::string& peerId, const std::string& fingerprint) {
        std::cout << "Peer request received from " << peerId << std::endl;

        if (approveAllIncomingRequests)
        {
            return true;
        }
        return false;
    });
    this->peeringAPI.OnNewPeer([this](const std::string& peerId, const std::string& signalingId, const std::string& fingerprint) {

        auto newPeer = std::make_shared<Peer>(peerId, signalingId, fingerprint, this->peeringAPI);
        this->HandleNewPeer(newPeer);

        // std::string tempUUID = UUID::generateUUID();
        // this->userDataManager.GetPeerMap_().AddUnprocessedPeer(tempUUID, newPeer);

        // newPeer->OnConnected([this, wpeer = std::weak_ptr(newPeer), tempUUID] {
        //     auto peer = wpeer.lock();
        //     if (!peer)
        //         return;
        //
        //     peer->SubscribeEventGeneric([this, wpeer = std::weak_ptr(peer)](const BaseMessage<MessageType>& message) {
        //         auto peer = wpeer.lock();
        //         if (!peer)
        //             return;
        //
        //         this->HandlePeerMessage(wpeer, message);
        //     });
        //
        //     peer->RequestPublicKey([this, wpeer = std::weak_ptr(wpeer), tempUUID](const std::string& publicKey) {
        //         auto peer = wpeer.lock();
        //         if (!peer)
        //             return;
        //         // if (const auto contactOpt = this->userDataManager.GetContactFromPublicKey(publicKey); contactOpt.has_value())
        //         // {
        //         peer->VerifyAuthenticity(publicKey, [this, wpeer = std::weak_ptr(wpeer), tempUUID](bool success) {
        //             auto peer = wpeer.lock();
        //             if (!peer)
        //                 return;
        //             if (success)
        //             {
        //                 this->userDataManager.GetPeerMap_().AcceptUnprocessedPeer(tempUUID);
        //             }
        //         });
        //         // }
        //     });
        //     peer->RequestProfile();
        // });
        // newPeer->OnDisconnected([this, wpeer = std::weak_ptr(newPeer)] {
        //     auto peer = wpeer.lock();
        //     if (!peer)
        //         return;
        //
        //     auto key = peer->GetVerfiedPublicKey();
        //     if (auto contactOpt = this->userDataManager.GetContactFromPublicKey(key); contactOpt.has_value())
        //     {
        //         auto contact = *contactOpt;
        //         contact.lastSeen = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        //         this->userDataManager.UpdateContact(key, contact);
        //         this->userDataManager.UpdateToLocalUsers();
        //     }
        // });
        // this->textChatAPI.OnChatMessage(newPeer->GetPeerId(), [this, wpeer = std::weak_ptr(newPeer)](const ChatMessageInfo& e) {
        //     auto peer = wpeer.lock();
        //     if (!peer)
        //         return;
        //
        //     this->IncrementPeerUnreadMessageCount(peer->GetFingerprint());
        //     this->AddChatMessageToPeerChatHistory(peer->GetFingerprint(), {e.content, e.timestamp, e.senderId, peer->GetProfile()});
        // });
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
    profile.description = "This user has nothing to say about themselves";
    profile.status = "This is " + username_ + "'s status";
    profile.nameColor = {1.0f, 1.0f, 1.0f};

    LocalUserData data{};
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
    for (const auto& peer: this->userDataManager.GetPeerMap_().GetPeers())
    {
        peer->SendMessage(UserProfileRequestResponse{userProfile});
    }
}

const std::vector<LocalUserData>& UserLogic::GetLocalUserInfos() const
{
    return this->localUsersAPI.GetLocalUserInfos();
}


const std::string& UserLogic::GetSelectedPeerId() const
{
    return this->userDataManager.GetSelectedChat();
}

void UserLogic::HandleRemoteUserMessage(std::shared_ptr<RemoteUser> remoteUser, std::shared_ptr<Peer> peer, const BaseMessage<MessageType>& message)
{
    if (!remoteUser || !peer)
        return;

    const std::string& peerId = peer->GetPeerId();
    auto& peerExchanges = peer->GetOngoingExchanges();

    switch (message.GetOpcode())
    {
        case ResetExchange::opcode: {
            auto exchangeType = static_cast<ExchangeType>(dynamic_cast<const class ResetExchange&>(message).exchangeType);
            std::cout << "Exchange of type " << static_cast<int>(exchangeType) << " was reset by " << peerId << std::endl;

            peerExchanges.ForceFail(exchangeType);
            break;
        }
        case TextRequest::opcode: {
            this->HandleTextRequest(peer);
            break;
        }
        case TextRequestResponse::opcode: {

            if (peerExchanges.SetExchangeState(ExchangeType::TextRequestExchange, OfferTextRequestExchange::CompletedState::Make()))
            {
                peer->SendMessage(TextResponseAck{});
            }
            break;
        }
        case TextResponseAck::opcode: {
            if (peerExchanges.SetExchangeState(ExchangeType::TextRequestExchange, AnswerTextRequestExchange::CompletedState::Make()))
            {
            }
            break;
        }
        case AuthChallengeRequest::opcode: {
            const auto authChallengeRequest = dynamic_cast<const class AuthChallengeRequest&>(message);
            std::cout << "Auth challenge request received from " << peerId << std::endl;

            const std::string decryptedToken = DecryptRSA(
                    authChallengeRequest.encryptedToken, this->userDataManager.GetDecryptedPrivateKey());

            std::cout << "Decrypted token: " << decryptedToken << std::endl;

            peer->SendMessage(AuthChallengeAnswer{decryptedToken});
            break;
        }
        case AuthChallengeAnswer::opcode: {

            const auto authChallengeAnswer = dynamic_cast<const class AuthChallengeAnswer&>(message);
            std::cout << "Auth challenge answer received from " << peerId << std::endl;

            std::cout << "Decrypted token: " << authChallengeAnswer.decryptedToken << std::endl;

            peerExchanges.SetExchangeState(ExchangeType::AuthChallengeExchange, OfferAuthChallengeRequest::CompletedState::Make(authChallengeAnswer.decryptedToken));

            break;
        }
        case ContactRequest::opcode: {
            const auto contactRequest = dynamic_cast<const class ContactRequest&>(message);
            std::cout << "Contact request received from " << peerId << std::endl;

            // TODO: add some kind of UI prompt to accept or reject the request
            peer->SetVerifiedPublicKey(contactRequest.publicIdentityKey);
            uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->userDataManager.AddContact(ContactData{peer->GetFingerprint(), contactRequest.publicIdentityKey, timestamp, peer->GetProfile()});
            peer->VerifyAuthenticity(contactRequest.publicIdentityKey);
            // peer->AddSystemLogToPeerChatHistory("Contact request received! You can now identify " + peerId + "!");
            peer->SendMessage(ContactRequestResponse{true, this->userDataManager.GetPublicIdentificationKey()});
            break;
        }
        case ContactRequestResponse::opcode: {
            const auto contactRequestResponse = dynamic_cast<const class ContactRequestResponse&>(message);

            if (peerExchanges.SetExchangeState(ExchangeType::ContactRequestExchange, OfferContactRequest::CompletedState::Make(contactRequestResponse)))
            {
                std::cout << "Contact request response received from " << peerId << std::endl;
            }
            break;
        }
        case UserProfileRequest::opcode: {
            peer->SendMessage(UserProfileRequestResponse{this->userDataManager.GetUserProfile()});
            break;
        }
        case UserProfileRequestResponse::opcode: {
            const auto peerProfileResponse = dynamic_cast<const class UserProfileRequestResponse&>(message);
            remoteUser->SetProfile(peerProfileResponse.userProfile);
            // peer->UpdateProfile(peerProfileResponse.userProfile);
            if (peer->IsAuthenticated())
            {
                const std::string& peerPubKey = peer->GetVerfiedPublicKey();
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
            peer->SendMessage(PublicKeyRequestResponse{this->userDataManager.GetPublicIdentificationKey()});
            break;
        }
        case PublicKeyRequestResponse::opcode: {
            const auto publicKeyResponse = dynamic_cast<const class PublicKeyRequestResponse&>(message);
            peerExchanges.SetExchangeState(ExchangeType::PublicKeyRequestExchange, OfferPublicKeyRequest::CompletedState::Make(publicKeyResponse.publicKey));
            break;
        }
        case ChannelRequest::opcode: {
            const auto channelRequest = dynamic_cast<const class ChannelRequest&>(message);
            std::cout << "Channel request received from " << peerId << std::endl;

            const std::string& channelUuid = channelRequest.channelUuid;
            const std::string& channelLocalName = channelRequest.channelName;
            const std::string& channelName = channelRequest.channelName;
            const std::string& channelDescription = channelRequest.channelDescription;
            const ChannelRequest::ChannelConfig& channelConfig = channelRequest.channelConfig;
            auto& presentUsers = channelRequest.presentUsers;
            auto& roles = channelRequest.roles;
            const std::string& userRole = channelRequest.userRole;

            if (auto& channelMap = this->userDataManager.GetChannelMap(); channelMap.ContainsChannel(channelUuid))
            {
                peer->SendMessage(ChannelRequestResponse{channelUuid, false});
            }
            else
            {
                auto channel = std::make_shared<Channel>(channelUuid, channelLocalName, channelName, channelDescription, ChannelConfig{channelConfig.maxMembers});

                std::vector<Role> channelRoles;
                for (auto& role : roles)
                {
                    std::unordered_set<Permission> rolePerms;
                    for (auto& perm : role.permissions)
                    {
                        rolePerms.emplace(static_cast<Permission>(perm));
                    }
                    channelRoles.emplace_back(role.name, std::move(rolePerms));
                }
                channel->InitExisting(peer->GetVerfiedPublicKey(), channelRoles, userRole);

                for (auto& member: presentUsers)
                {
                    // if (member.fingerprint != peer->GetFingerprint())
                    // {
                        if (auto memberRemoteUser = this->userDataManager.GetRemoteUserMap().Get(member.fingerprint))
                        {
                            channel->AddMember(memberRemoteUser, member.role, false);
                        }
                        else
                        {
                            memberRemoteUser = this->AddRemoteUser(member.username, member.fingerprint);
                            channel->AddMember(memberRemoteUser, member.role, false);
                            // this->QueryUserFingerprint(member.fingerprint, [this, member, channel](nlohmann::json json) {
                            //     const auto signalingId = json[0]["signalingId"].get<std::string>();
                            //     this->AddNewChatPeer(signalingId, [channel, member](const std::weak_ptr<Peer>& wpeer) {
                            //         const auto memberRemoteUser = wpeer.lock();
                            //         if (!memberRemoteUser)
                            //             return;
                            //
                            //         channel->SetMemberActiveUser(member.fingerprint, memberRemoteUser);
                            //     });
                            // });
                        }
                    // }
                }
                channelMap.AddChannel(channel);
                peer->SendMessage(ChannelRequestResponse{channelUuid, true});
            }
            break;
        }
        case ChannelRequestResponse::opcode: {
            const auto channelResponse = dynamic_cast<const class ChannelRequestResponse&>(message);
            std::cout << "Channel request response received from " << peerId << std::endl;

            peerExchanges.SetExchangeState(ExchangeType::ChannelRequestExchange, OfferChannelRequest::CompletedState::Make(channelResponse));

            // const std::string& channelUuid = channelResponse.channelUuid;
            // bool accepted = channelResponse.accepted;

            // if (accepted)
            // {
                // const auto& channelMap = this->userDataManager.GetChannelMap();
                // auto channel = channelMap.GetChannel(channelUuid);
            // }
            break;
        }
        case ChannelUserJoin::opcode: {
            const auto channelUserJoin = dynamic_cast<const class ChannelUserJoin&>(message);
            std::cout << "Channel user join received from " << peerId << std::endl;

            if (channelUserJoin.newChannelMember.fingerprint == Hashing::GeneratePublicKeyFingerprint(this->userDataManager.GetPublicIdentificationKey()))
                break;

            auto channel = this->userDataManager.GetChannelMap().GetChannel(channelUserJoin.channelUuid);
            if (channel)
            {
                if (auto memberRemoteUser = this->userDataManager.GetRemoteUserMap().Get(channelUserJoin.newChannelMember.fingerprint))
                {
                    channel->AddMember(memberRemoteUser, channelUserJoin.newChannelMember.role, false);
                }
                else
                {
                    memberRemoteUser = this->AddRemoteUser(channelUserJoin.newChannelMember.username, channelUserJoin.newChannelMember.fingerprint);
                    channel->AddMember(memberRemoteUser, channelUserJoin.newChannelMember.role, false);
                }
            }
        }
        case SignedMessage::opcode: {
            break;
        }
        default: {
            break;
        }
    }
}

void UserLogic::HandleNewPeer(std::shared_ptr<Peer> peer)
{
    PeerMap& peerMap = this->userDataManager.GetPeerMap_();

    std::string tempUuid = UUID::generateUUID();
    peerMap.AddUnprocessedPeer(tempUuid, peer);

    // listen to the messages necessary to authenticate the peer
    peer->SubscribeEventOnce<PublicKeyRequest>([this, wpeer = std::weak_ptr(peer)] (auto _) {
        auto peer = wpeer.lock();
        if (!peer)
            return;

        peer->SendMessage(PublicKeyRequestResponse{this->userDataManager.GetPublicIdentificationKey()});
    });
    peer->SubscribeEventOnce<PublicKeyRequestResponse>([wpeer = std::weak_ptr(peer)](const PublicKeyRequestResponse& e) {
        auto peer = wpeer.lock();
        if (!peer)
            return;

        peer->GetOngoingExchanges().SetExchangeState(ExchangeType::PublicKeyRequestExchange, OfferPublicKeyRequest::CompletedState::Make(e.publicKey));
    });

    peer->SubscribeEventOnce<AuthChallengeRequest>([this, wpeer = std::weak_ptr(peer)](const AuthChallengeRequest& e) {
        auto peer = wpeer.lock();
        if (!peer)
            return;

        std::cout << "Auth challenge request received from " << peer->GetPeerId() << std::endl;

        const std::string decryptedToken = DecryptRSA(
                e.encryptedToken, this->userDataManager.GetDecryptedPrivateKey());

        std::cout << "Decrypted token: " << decryptedToken << std::endl;

        peer->SendMessage(AuthChallengeAnswer{decryptedToken});
    });
    peer->SubscribeEventOnce<AuthChallengeAnswer>([wpeer = std::weak_ptr(peer)](const AuthChallengeAnswer& e) {
        auto peer = wpeer.lock();
        if (!peer)
            return;

        peer->GetOngoingExchanges().SetExchangeState(ExchangeType::AuthChallengeExchange, OfferAuthChallengeRequest::CompletedState::Make(e.decryptedToken));
    });

    peer->OnConnected([this, wpeer = std::weak_ptr(peer), tempUuid] {
        auto peer = wpeer.lock();
        if (!peer)
            return;

        peer->ValidateConnection([this, tempUuid](bool success, std::string publicKey) {

            if (success)
            {
                std::string verifiedFingerprint = Hashing::GeneratePublicKeyFingerprint(publicKey);
                // std::string username;
                auto peer = this->userDataManager.GetPeerMap_().AcceptUnprocessedPeer(tempUuid);
                if (peer)
                {
                    peer->SetVerifiedPublicKey(publicKey);
                    peer->SetFingerprint(verifiedFingerprint);
                    RegisterPeerRemoteUser(peer, verifiedFingerprint);
                }
            }
            else
            {
                this->userDataManager.GetPeerMap_().RejectUnprocessedPeer(tempUuid);
            }
        });
    });

}

// void UserLogic::HandlePeerSignedMessage(const std::weak_ptr<Peer>& wpeer, const SignedMessage& message)
// {
//     auto peer = wpeer.lock();
//     if (!peer)
//         return;
//
//     const std::string& peerId = peer->GetPeerId();
//     auto& peerExchanges = peer->GetOngoingExchanges();
//
//     MessageType opcode;
//     std::vector<std::byte> payload;
//     zpp::bits::in in(message.message);
//     in(opcode, payload).or_throw();
//
//     switch (opcode)
//     {
//         default:
//             break;
//     }
// }

void UserLogic::InitiateTextRequest(std::shared_ptr<Peer> peer)
{
    auto exchange = [wpeer = std::weak_ptr(peer), this](bool success) {
        auto peer = wpeer.lock();
        if (!peer)
            return;
        if (!success)
        {
            peer->SendMessage(ResetExchange{ExchangeType::TextRequestExchange});
        }
        else
        {
            std::cout << "Exchange of type " << static_cast<int>(ExchangeType::TextRequestExchange) << " with " << peer->GetPeerId() << " was successful" << std::endl;
            this->textChatAPI.InitiateTextChat(peer->GetPeerId());
        }
    };

    peer->GetOngoingExchanges().StartNewExchange<OfferTextRequestExchange>(std::move(exchange));

    peer->SendMessage(TextRequest{});
    // CreateNewChatHistory(peerId);
}

void UserLogic::HandleTextRequest(std::shared_ptr<Peer> peer)
{
    auto exchange = [wpeer = std::weak_ptr(peer), this](bool success) {
        auto peer = wpeer.lock();
        if (!peer)
            return;
        if (!success)
        {
            peer->SendMessage(ResetExchange{ExchangeType::TextRequestExchange});
        }
        else
        {
            std::cout << "Exchange of type " << static_cast<int>(ExchangeType::TextRequestExchange) << " with " << peer << " was successful" << std::endl;
            this->textChatAPI.InitiateTextChat(peer->GetPeerId());
        }
        // peerExchanges->EndExchange(ExchangeType::TextRequestExchange);
    };
    if (peer->GetOngoingExchanges().StartNewExchange<AnswerTextRequestExchange>(std::move(exchange)).first)
    {
        peer->SendMessage(TextRequestResponse{});
    }
}