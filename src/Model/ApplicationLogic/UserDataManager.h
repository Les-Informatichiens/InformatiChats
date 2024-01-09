//
// Created by Jonathan Richard on 2023-10-02.
//

#pragma once

#include "Model/DataAccess/ILocalUsersAPI.h"
#include "Model/Models/User.h"
#include "Model/Models/UserData.h"
#include "PeerManager.h"


#include <ranges>


class UserDataManager
{
public:
    explicit UserDataManager(User& user_, ILocalUsersAPI& localUsersAPI_) : user(user_), localUsersAPI(localUsersAPI_) {};

    // Manage user-related data
    void Reset(const std::string& username) const { this->user = User{username}; };
    void Reset(const UserData& userData) const { this->user = User{userData}; };
    [[nodiscard]] const std::string& GetUsername() const { return this->user.userData.permanentUsername; };

    [[nodiscard]] const std::string& GetSelectedChat() const { return this->user.selectedChat; };
    void SetSelectedChat(const std::string& peerId) const { this->user.selectedChat = peerId; };


    // Manage peer-related data

    [[nodiscard]] const ChatHistory* GetSelectedChatHistory() const
    {
        if (!this->GetSelectedChat().empty() && !this->user.peerMap.empty())
        {
            return &this->user.peerMap.at(this->user.selectedChat).chatHistory;
        }
        return nullptr;
    }


    void AppendSelectedChatHistory(const std::string& message) const
    {
        const auto peer = this->user.peerMap.find(this->user.selectedChat);

        if (peer != this->user.peerMap.end())
        {
            peer->second.chatHistory.emplace_back(
                    message,
                    duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                    this->user.userData.permanentUsername,
                    this->user.userData.profile);
        }
    }

    void AddChatMessageToPeerChatHistory(const std::string& peerId, const ChatMessageInfo& chatMessage) const
    {
        const auto peer = this->user.peerMap.find(peerId);

        if (peer != this->user.peerMap.end())
        {
            peer->second.chatHistory.emplace_back(chatMessage);
        }
    }

    void UpdatePeerState(const std::string& peerId, const ConnectionState& state) const
    {
        //    auto result = this->user.peerMap.insert({peerId, {}});
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            peerIt->second.peerData.connectionState = state;
        }
    }

    void UpdatePeerIpAddress(const std::string& peerId, const std::string& ipAddress) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            peerIt->second.ipAddress = ipAddress;
        }
    }

    [[nodiscard]] std::string GetPeerIpAddress(const std::string& peerId) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            return peerIt->second.ipAddress;
        }
        return "";
    }

    void UpdatePeerVerificationStatus(const std::string& peerId, bool verified) const
    {
        //    auto result = this->user.peerMap.insert({peerId, {}});
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            peerIt->second.peerData.authenticated = verified;
        }
    }

    void IncrementPeerUnreadMessageCount(const std::string& peerId) const
    {
        ++this->user.peerMap.at(peerId).peerData.unreadMessageCount;
    }

    [[nodiscard]] std::unordered_map<std::string, PeerData> GetPeerDataMap() const
    {
        std::unordered_map<std::string, PeerData> peerDataMap;

        for (auto& peer: this->user.peerMap)
        {
            peerDataMap.insert({peer.first, peer.second.peerData});
        }
        return peerDataMap;
    }

    [[nodiscard]] std::optional<PeerData> GetPeerData(const std::string& peerId) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            return {peerIt->second.peerData};
        }
        return std::nullopt;
    }


    void AddNewPeer(const std::string& peerId) const
    {
        this->user.peerMap.emplace(peerId, peerId);
    }

    void RemovePeer(const std::string& peerId) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            this->user.peerMap.erase(peerIt);
        }
    }

    [[nodiscard]] ExchangeManager* GetPeerExchanges(const std::string& peerId) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            return &peerIt->second.ongoingExchanges;
        }
        return nullptr;
    }

    void UpdateToLocalUsers() const
    {
        this->localUsersAPI.UpdateLocalUser(this->user.userData);
        this->localUsersAPI.SaveLocalUserInfos();
    }

    void SetPeerVerificationKey(const std::string& peerId, const std::string& verificationKey) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            peerIt->second.verifiedPublicKey = verificationKey;
        }
    }

    [[nodiscard]] std::string GetPeerVerificationKey(const std::string& peerId) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            return peerIt->second.verifiedPublicKey;
        }
        return "";
    }

    [[nodiscard]] const std::string& GetDecryptedPrivateKey() const
    {
        return this->user.decryptedPrivateKey;
    }

    void SetDecryptedPrivateKey(const std::string& privateKey) const
    {
        this->user.decryptedPrivateKey = privateKey;
    }

    [[nodiscard]] const std::string& GetPublicIdentificationKey() const
    {
        return this->user.userData.publicIdentificationKey;
    }

    void AddContact(ContactData contact) const
    {
        this->user.userData.contacts.emplace(contact.publicIdentificationKey, std::move(contact));
        this->UpdateToLocalUsers();
    }

    void RemoveContact(const std::string& publicKey) const
    {
        if (this->user.userData.contacts.contains(publicKey))
        {
            this->user.userData.contacts.erase(publicKey);
        }
        this->UpdateToLocalUsers();
    }

    void UpdateContact(const std::string& publicKey, const ContactData& contact) const
    {
        this->user.userData.contacts.insert_or_assign(publicKey, contact);
    }

    [[nodiscard]] std::optional<ContactData> GetContactFromPublicKey(const std::string& publicKey) const
    {
        auto it = this->user.userData.contacts.find(publicKey);
        if (it != this->user.userData.contacts.end())
        {
            return {it->second};
        }
        return std::nullopt;
    }

    [[nodiscard]] std::vector<ContactData> GetContactsFromPeerId(const std::string& peerId) const
    {
        std::vector<ContactData> contacts;
        for (auto& contact: this->user.userData.contacts | std::views::values)
        {
            if (contact.permanentUsername == peerId)
            {
                contacts.push_back(contact);
            }
        }
        return contacts;
    }

    [[nodiscard]] bool IsContactKeyAlreadyRegistered(const std::string& publicKey) const
    {
        return this->user.userData.contacts.contains(publicKey);
    }


    [[nodiscard]] const UserProfile& GetUserProfile() const
    {
        return this->user.userData.profile;
    }

    void UpdateUserProfile(const UserProfile& newProfile) const
    {
        this->user.userData.profile = newProfile;
        this->UpdateToLocalUsers();
    }

    void UpdatePeerProfile(const std::string& peerId, const UserProfile& profile) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            peerIt->second.peerData.profile = profile;
        }
    }

    [[nodiscard]] const UserProfile& GetPeerProfile(const std::string& peerId) const
    {
        const auto peerIt = this->user.peerMap.find(peerId);
        if (peerIt != this->user.peerMap.end())
        {
            return peerIt->second.peerData.profile;
        }
        return this->user.userData.profile;
    }

private:
    User& user;
    ILocalUsersAPI& localUsersAPI;

    // TODO: decide if this is useful
//    PeerManager& peerDataManager;
};
