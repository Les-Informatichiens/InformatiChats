//
// Created by Jean on 8/29/2023.
//
#pragma once

#include "Model/DataAccess/IConnectionAPI.h"
#include "Model/DataAccess/IPeeringAPI.h"
#include "Model/DataAccess/ITextChatAPI.h"
#include <Model/DataAccess/ILocalUsersAPI.h>
#include <Model/ApplicationLogic/UserDataManager.h>

class UserLogic
{
public:
    UserLogic(UserDataManager& userDataManager, IConnectionAPI& connectionAPI, IPeeringAPI& peeringAPI, ITextChatAPI& textChatAPI, ILocalUsersAPI& localUsersAPI)
        : userDataManager(userDataManager), connectionAPI(connectionAPI), peeringAPI(peeringAPI), textChatAPI(textChatAPI), localUsersAPI(localUsersAPI){};

    void Reset(const UserData& userData);
    [[nodiscard]] bool IsClientConnected() const;

    bool LoginWithNewUser(const std::string& username, const std::string& password);
    bool CreateNewUser(const std::string& username, const std::string& password);
    void LoadLocalUsers() const;
    void UpdateUserProfile(const UserProfile& userProfile) const;

    void SendTextMessage(const std::string& message);
    void AddNewChatPeer(const std::string& peerId);
    void SetSelectedPeerId(const std::string& peerId);
    void UpdatePeerState(const std::string& peerId, const ConnectionState& state);
    void IncrementPeerUnreadMessageCount(const std::string& peerId);
    void VerifyPeerAuthentication(const std::string& peerId, const std::string& publicKey);
    void AddPeerAsContact(const std::string& peerId);
    void RemovePeerFromContacts(const std::string& string);

    void CreateNewChatHistory(const std::string& peerId);
    void AppendSelectedChatHistory(const std::string& message) const;
    void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessageInfo& chatMessage) const;
    void AddSystemLogToPeerChatHistory(const std::string& peerId, const std::string& message, std::array<float, 3> color) const;

    [[nodiscard]] std::unordered_map<std::string, PeerData> GetPeerDataMap() const;
    [[nodiscard]] const ChatHistory* GetSelectedChatHistory() const;
    [[nodiscard]] const std::vector<UserData>& GetLocalUserInfos() const;
    [[nodiscard]] const std::string& GetUserName() const;
    [[nodiscard]] static const size_t& GetMaxNameLength();
    [[nodiscard]] const std::string& GetSelectedPeerId() const;
    [[nodiscard]] const UserDataManager& GetUserDataManager() const {return userDataManager;};

private:
    void HandlePeerMessage(const std::string& peerId, const BaseMessage<MessageType>& message);

    void InitiateTextRequest(const std::string& peerId);
    void HandleTextRequest(const std::string& peerId);

    void InterrogatePeerPublicKey(const std::string& peerId, std::function<void(std::string)> onKeyReceived) const;

private:
    UserDataManager& userDataManager;
    IConnectionAPI& connectionAPI;
    IPeeringAPI& peeringAPI;
    ITextChatAPI& textChatAPI;
    ILocalUsersAPI& localUsersAPI;
};
