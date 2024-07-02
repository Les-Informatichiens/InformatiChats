//
// Created by Jonathan Richard on 2024-01-06.
//

#pragma once

#include "Peer.h"
#include "RemoteUser.h"
#include "util/crypto/Hashing.h"


#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>


struct ChannelConfig
{
    int16_t maxMembers = 8;
};

enum class Permission {
    OWNER,
    ADMIN,
    ADD_USER,
    REMOVE_USER,
    BAN_USER,
    MUTE_USER,
    UNMUTE_USER,
    TEXT,
    AUDIO,
    VIDEO,
    SCREENSHARE,
};

struct Role {
    std::string name;
    std::unordered_set<Permission> permissions;
};

class PredefinedRoles
{
public:
    static Role Owner() {
        return Role{"Owner", {
            Permission::OWNER,
        }};
    }

    static Role Admin() {
        return Role{"Admin", {
            Permission::ADMIN,
        }};
    }

    static Role User() {
        return Role{"User", {
            Permission::TEXT,
            Permission::AUDIO,
            Permission::VIDEO,
            Permission::SCREENSHARE,
        }};
    }

    static Role Guest() {
        return Role{"Guest", {
            Permission::TEXT,
        }};
    }

    static Role DM() {
        return Role{"DM", {
            Permission::TEXT,
            Permission::AUDIO,
            Permission::VIDEO,
            Permission::SCREENSHARE,
        }};
    }
};

class ChannelMember
{
public:
    // explicit ChannelMember(const std::vector<std::byte>& data)
    // {
        // this->Deserialize(data);
    // }

    ChannelMember(const std::shared_ptr<RemoteUser>& remoteUser)
        : remoteUser(remoteUser)
    {
    }

    // ChannelMember(const std::shared_ptr<RemoteUser>& remoteUser, Role role)
        // : remoteUser(remoteUser), fingerprint(remoteUser->GetFingerprint()), role(std::move(role))
    // {
    // }

    // ChannelMember(std::string publicKey, Role role)
        // : fingerprint(std::move(Hashing::GeneratePublicKeyFingerprint(publicKey))), publicKey(std::move(publicKey)), role(std::move(role))
    // {
    // }

    void AddRole(const std::string& role) {
        this->roles.emplace(role);
    }

    void RemoveRole(const std::string& role) {
        this->roles.erase(role);
    }

    void SetActiveUser(const std::shared_ptr<RemoteUser>& remoteUser) {
        this->remoteUser = remoteUser;
    }
    //
    // [[nodiscard]] bool HasPermission(Permission permission) const {
    //     for (const auto& role: this->roles) {
    //         if (role.permissions.contains(permission)) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    // [[nodiscard]] const Role& GetRole() const
    // {
    //     return this->role;
    // }

    [[nodiscard]] const std::set<std::string>& GetRoles() const
    {
        return this->roles;
    }

    [[nodiscard]] std::weak_ptr<RemoteUser> GetRemoteUser() const
    {
        return this->remoteUser;
    }

    // [[nodiscard]] std::vector<std::byte> Serialize() const
    // {
    //     auto [data, out] = zpp::bits::data_out();
    //     out(this->fingerprint, this->publicKey, this->role).or_throw();
    //     return data;
    // };
    // void Deserialize(const std::vector<std::byte>& data)
    // {
    //     zpp::bits::in in(data);
    //     in(this->fingerprint, this->publicKey, this->role).or_throw();
    // };

private:
    std::set<std::string> roles;
    std::weak_ptr<RemoteUser> remoteUser;
};

using ChatHistory = std::vector<ChatMessageInfo>;

class Channel
{
public:
    Channel(std::string uuid, std::string localName, std::string name, std::string description)
        : uuid(std::move(uuid)), localName(std::move(localName)), name(std::move(name)), description(std::move(description)), config({})
    {
    }
    Channel(std::string uuid, std::string localName, std::string name, std::string description, ChannelConfig config)
        : uuid(std::move(uuid)), localName(std::move(localName)), name(std::move(name)), description(std::move(description)), config(config)
    {
    }

    void AddRoles(const std::vector<Role>& roles) {
        for (const auto& role: roles) {
            this->roles.emplace(role.name, role);
        }
    }

    void InitExisting(const std::string& publicKey, const std::vector<Role>& roles, const std::string& role)
    {
        if (this->isInitialized)
            return;

        this->AddRoles(roles);
        this->ownRole = this->roles.at(role);

        this->ownFingerprint = Hashing::GeneratePublicKeyFingerprint(publicKey);
        this->ownPublicKey = publicKey;

        this->members.clear();

        this->isInitialized = true;
    }

    void InitNew(const std::string& publicKey, const std::vector<Role>& roles, const std::string& role)
    {
        if (this->isInitialized)
            return;

        this->AddRoles(roles);
        this->ownRole = this->roles.at(role);

        this->ownFingerprint = Hashing::GeneratePublicKeyFingerprint(publicKey);
        this->ownPublicKey = publicKey;

        this->members.clear();

        this->isInitialized = true;
    }

    // void AddMember(const std::string& publicKey, const std::string& fingerprint, const std::string& roleName, bool commitAction)
    // {
    //     if (members.size() < config.maxMembers) {
    //         auto roleIt = roles.find(roleName);
    //         if (roleIt != roles.end()) {
    //             ChannelMember newMember(publicKey, roleIt->second);
    //             auto res = members.emplace(fingerprint, std::move(newMember));
    //             if (res.second && commitAction)
    //             {
    //                 this->CommitAddMember(fingerprint);
    //             }
    //         }
    //     }
    // }

    void AddMember(const std::shared_ptr<RemoteUser>& remoteUser, const std::string& role, bool commitAction)
    {
        if (members.size() < config.maxMembers - 1) {
            auto roleIt = roles.find(role);
            if (roleIt != roles.end()) {
                ChannelMember newMember(remoteUser, roleIt->second);
                auto res = members.emplace(remoteUser->GetFingerprint(), std::move(newMember));
                if (res.second && commitAction)
                {
                    this->CommitAddMember(remoteUser->GetFingerprint());
                }
            }
        }
    }

    void SetMemberActiveUser(const std::string& fingerprint, const std::shared_ptr<RemoteUser>& remoteUser) {
        if (const auto it = this->members.find(fingerprint); it != this->members.end()) {
            auto& member = it->second;
            member.SetActiveUser(remoteUser);
        }
    }

    void AssignRoleToMember(const std::string& fingerprint, const std::string& roleName) {
        if (auto it = this->members.find(fingerprint); it != this->members.end()) {
            auto& member = it->second;
            if (auto roleIt = this->roles.find(roleName); roleIt != this->roles.end()) {
                member.AddRole(roleIt->first);
            }
        }
    }

    void RemoveRoleFromMember(const std::string& fingerprint, const std::string& roleName) {
        if (auto it = this->members.find(fingerprint); it != this->members.end()) {
            auto& member = it->second;
            if (auto roleIt = this->roles.find(roleName); roleIt != this->roles.end()) {
                member.RemoveRole(roleIt->first);
            }
        }
    }

    bool MemberHasPermission(const std::string& fingerprint, Permission permission) const {
        if (const auto it = this->members.find(fingerprint); it != this->members.end()) {
            const auto& member = it->second;
            for (const auto& role: member.GetRoles()) {
                if (auto roleIt = this->roles.find(role); roleIt != this->roles.end()) {
                    if (roleIt->second.permissions.contains(permission)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    [[nodiscard]] ChannelRequest CreateChannelRequest(std::string role) const
    {
        const ChannelRequest::ChannelConfig config { this->config.maxMembers };
        std::vector<ChannelRequest::ChannelMember> members;
        for (const auto& [fingerprint, member]: this->members) {
            members.emplace_back(fingerprint, std::vector(member.GetRoles().begin(), member.GetRoles().end()));
        }
        members.push_back({this->ownFingerprint, std::vector(this->ownRoles.begin(), this->ownRoles.end())});
        std::vector<ChannelRequest::Role> roles;
        for (const auto& [name, role]: this->roles) {
            std::vector<int32_t> permissions;
            for (const auto& permission: role.permissions) {
                permissions.push_back(static_cast<int32_t>(permission));
            }
            roles.emplace_back(name, permissions);
        }

        return ChannelRequest(this->uuid, this->name, this->description, config, members, roles, role);
    }

    void AddSystemLogToPeerChatHistory(const std::string& message, std::array<float, 3> color = {0.4, 1.0f, 0.4f})
    {
        this->AddChatMessageToPeerChatHistory({message,
                                               duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                                               "System",
                                               {"System",
                                                "System",
                                                "System",
                                                color}});
    }

    void AddChatMessageToPeerChatHistory(const ChatMessageInfo& chatMessage)
    {
        this->chatHistory.emplace_back(chatMessage);
    }

    [[nodiscard]] const std::string& GetUuid() const
    {
        return this->uuid;
    }
    [[nodiscard]] const std::string& GetName() const
    {
        return this->name;
    }
    [[nodiscard]] const std::string& GetDescription() const
    {
        return this->description;
    }
    [[nodiscard]] const ChannelConfig& GetConfig() const
    {
        return this->config;
    }

    [[nodiscard]] const std::unordered_map<std::string, ChannelMember>& GetMembers()
    {
        return members;
    }

    [[nodiscard]] const ChatHistory& GetChatHistory() const
    {
        return this->chatHistory;
    }

    [[nodiscard]] bool IsInitialized() const
    {
        return this->isInitialized;
    }

private:

    void CommitAddMember(const std::string& fingerprint)
    {
        for (const auto& member: this->members | std::views::values) {
            if (auto peer = member.GetRemoteUser().lock(); peer && peer->IsOnline())
            {
                peer->GetActivePeer()->SendMessage(ChannelUserJoin{this->uuid, ChannelUserJoin::ChannelMember{fingerprint, std::vector(member.GetRoles().begin(), member.GetRoles().end())}});
            }
        }
    }

private:

    std::string uuid;
    std::string localName;
    std::string name;
    std::string description;
    ChannelConfig config;
    std::unordered_map<std::string, ChannelMember> members;
    std::unordered_map<std::string, Role> roles;

    ChatHistory chatHistory;

    std::string ownFingerprint;
    std::string ownPublicKey;
    std::set<std::string> ownRoles;

    bool isInitialized = false;

};