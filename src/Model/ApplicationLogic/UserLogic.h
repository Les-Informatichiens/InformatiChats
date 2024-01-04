//
// Created by Jean on 8/29/2023.
//
#pragma once

#include <Model/DataAccess/IChatAPI.h>
#include <Model/DataAccess/ILocalUsersAPI.h>
#include <Model/Models/User.h>

class UserLogic
{
public:
    UserLogic(User& user, IChatAPI& chatAPI, ILocalUsersAPI& localUsersAPI)
        : user(user), chatAPI(chatAPI), localUsersAPI(localUsersAPI){};

    void Reset(const std::string& username);
    [[nodiscard]] bool IsClientConnected() const;

    bool LoginWithNewUser(const std::string& username, const std::string& password);
    bool CreateNewUser(const std::string& username, const std::string& password);
    void LoadLocalUsers() const;

    void SendTextMessage(const std::string& message);
    void AddNewChatPeer(const std::string& peerId);
    void SetSelectedPeerId(const std::string& peerId);
    void UpdatePeerState(const std::string& peerId, const ConnectionState& state);
    void IncrementPeerUnreadMessageCount(const std::string& peerId);

    void CreateNewChatHistory(const std::string& peerId);
    void AppendSelectedChatHistory(const std::string& message);
    void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage);

    [[nodiscard]] const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const;
    [[nodiscard]] const ChatHistory* GetSelectedChatHistory() const;
    [[nodiscard]] const std::vector<UserData>& GetLocalUserInfos() const;
    [[nodiscard]] const std::string& GetUserName() const;
    [[nodiscard]] static const size_t& GetMaxNameLength();
    [[nodiscard]] const std::string& GetSelectedPeerId() const;

private:
    User& user;
    IChatAPI& chatAPI;
    ILocalUsersAPI& localUsersAPI;
};
