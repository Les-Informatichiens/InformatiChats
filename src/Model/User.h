#pragma once

#include "ChatClient.h"
#include "IUser.h"
#include "PeerData.h"


class User : public IUser
{
public:
    User() = default;
    explicit User(std::string username);

    void Reset(std::string username) override;

    //void SetUsername(std::string newUsername);
    [[nodiscard]] std::string GetUsername() const override;
    [[nodiscard]] const std::unordered_map<std::string, PeerData>& GetPeerDataMap() const override;

    void SetSelectedPeerId(std::string selectedChat) override;
    const std::string& GetSelectedPeerId() const override;
    const ChatHistory* GetSelectedChatHistory() const override;
    void AddChatMessageToSelectedChatHistory(ChatMessage chatMessage) override;
    void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessage& chatMessage) override;
    void CreateNewChatHistory(const std::string& peerId) override;

    void UpdatePeerState(const std::string& peerId, ConnectionState state) override;
    void IncrementPeerUnreadMessageCount(const std::string& peerId) override;

private:
    ChatHistory* GetSelectedChatHistory();

private:
    std::string username;

    std::unordered_map<std::string, PeerData> peerDataMap;

    std::string selectedChat;

    // TODO: better management for chat histories
    // peerId, History for that peer id
    std::unordered_map<std::string, ChatHistory> chatHistories;
};
