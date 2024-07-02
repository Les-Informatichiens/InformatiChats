//
// Created by Jean on 8/29/2023.
//
#pragma once

#include "Model/DataAccess/IConnectionAPI.h"
#include "Model/DataAccess/IPeeringAPI.h"
#include "Model/DataAccess/ITextChatAPI.h"
#include <Model/ApplicationLogic/UserDataManager.h>
#include <Model/DataAccess/ILocalUsersAPI.h>
#include <promise-cpp/promise.hpp>

class UserLogic
{
public:
    UserLogic(UserDataManager& userDataManager, IConnectionAPI& connectionAPI, IPeeringAPI& peeringAPI, ITextChatAPI& textChatAPI, ILocalUsersAPI& localUsersAPI)
        : userDataManager(userDataManager), connectionAPI(connectionAPI), peeringAPI(peeringAPI), textChatAPI(textChatAPI), localUsersAPI(localUsersAPI){};

    void Reset(const LocalUserData& userData);
    [[nodiscard]] bool IsClientConnected() const;

    bool LoginWithNewUser(const std::string& username, const std::string& password);
    bool CreateNewUser(const std::string& username, const std::string& password);
    void LoadLocalUsers() const;
    void UpdateUserProfile(const UserProfile& userProfile) const;

    void SendTextMessage(const std::string& destinationChannel, const std::string& message);
    // void AddNewChatPeer(const std::string& signalingId, const std::function<void(std::weak_ptr<Peer>)>& callback = nullptr);
    std::shared_ptr<RemoteUser> AddRemoteUser(const std::string& username, const std::string& fingerprint);
    void RegisterPeerRemoteUser(std::shared_ptr<Peer> peer, const std::string& fingerprint);
    void QueryUserFingerprint(const std::string& fingerprint, std::function<void(nlohmann::json)>);
    // promise::Promise QueryUserFingerprint(const std::string& fingerprint);
    void SetSelectedPeerId(const std::string& peerId);
    void UpdatePeerState(const std::string& peerId, const ConnectionState& state);
    void IncrementPeerUnreadMessageCount(const std::string& peerId);
    void AddPeerAsContact(const std::string& peerId);
    void RemovePeerFromContacts(const std::string& string);

    std::string CreateUserDMChannel(const std::shared_ptr<RemoteUser>& remoteUser);
    void AddRemoteUserToChannel(const std::string& channelUuid, const std::shared_ptr<RemoteUser>& remoteUser, const std::string& role);

    void AppendSelectedChatHistory(const std::string& message) const;
    void AddChatMessageToChannelChatHistory(const std::string& channelUuid, const ChatMessageInfo& chatMessage) const;
    void AddSystemLogToChannelChatHistory(const std::string& channelUuid, const std::string& message, std::array<float, 3> color) const;

    // [[nodiscard]] std::unordered_map<std::string, PeerData> GetPeerDataMap() const;
    [[nodiscard]] const std::unordered_map<std::string, Peer>& GetPeerMap() const;
    [[nodiscard]] const std::unordered_map<std::string, ContactData>& GetContactMap() const;
    [[nodiscard]] const ChatHistory* GetSelectedChatHistory() const;
    [[nodiscard]] const std::vector<LocalUserData>& GetLocalUserInfos() const;
    [[nodiscard]] const std::string& GetUserName() const;
    [[nodiscard]] static const size_t& GetMaxNameLength();
    [[nodiscard]] const std::string& GetSelectedPeerId() const;
    [[nodiscard]] const UserDataManager& GetUserDataManager() const {return userDataManager;};

private:
    void HandleRemoteUserMessage(std::shared_ptr<RemoteUser> remoteUser, std::shared_ptr<Peer> peer, const BaseMessage<MessageType>& message);
    void HandleNewPeer(std::shared_ptr<Peer> peer);
    // void HandlePeerSignedMessage(const std::weak_ptr<Peer>& wpeer, const SignedMessage& message);

    void InitiateTextRequest(std::shared_ptr<Peer> peerId);
    void HandleTextRequest(std::shared_ptr<Peer> peer);

private:
    UserDataManager& userDataManager;
    IConnectionAPI& connectionAPI;
    IPeeringAPI& peeringAPI;
    ITextChatAPI& textChatAPI;
    ILocalUsersAPI& localUsersAPI;
};
