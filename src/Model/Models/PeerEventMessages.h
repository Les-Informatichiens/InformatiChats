//
// Created by Jonathan Richard on 2023-09-24.
//

#pragma once

#include "zpp_bits.h"
#include <Model/MessageDispatcher.h>
#include <Model/Models/LocalUserData.h>

#include <utility>


enum class MessageType
{
    Reset,
    TextRequest,
    TextRequestResponse,
    TextResponseAck,
    AuthChallengeRequest,
    AuthChallengeAnswer,
    ContactRequest,
    ContactRequestResponse,
    UserProfileRequest,
    UserProfileRequestResponse,
    PublicKeyRequest,
    PublicKeyRequestResponse,
    ChannelRequest,
    ChannelRequestResponse,
    ChannelUserJoin,
    ChannelUserLeave,
    ChannelActionProposal,
    SignedMessage,
    SendLocalDescription,
    SendLocalCandidate,
    ReceiveRemoteDescription,
    ReceiveRemoteCandidate,
};

class TextRequest final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::TextRequest;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};

class TextRequestResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::TextRequestResponse;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};

class TextResponseAck final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::TextResponseAck;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};

class AuthChallengeRequest final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::AuthChallengeRequest;

    mutable std::string encryptedToken;

public:
    AuthChallengeRequest() = default;
    explicit AuthChallengeRequest(auto encryptedToken_) : encryptedToken(encryptedToken_) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->encryptedToken).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->encryptedToken).or_throw();
    };
};

class AuthChallengeAnswer final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::AuthChallengeAnswer;

    mutable std::string decryptedToken;

public:
    AuthChallengeAnswer() = default;
    explicit AuthChallengeAnswer(auto decryptedToken_) : decryptedToken(decryptedToken_) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->decryptedToken).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->decryptedToken).or_throw();
    };
};

class ContactRequest final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ContactRequest;

    mutable std::string publicIdentityKey;

public:
    ContactRequest() = default;
    explicit ContactRequest(std::string publicIdentityKey_) : publicIdentityKey(std::move(publicIdentityKey_)) {};
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(publicIdentityKey).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->publicIdentityKey).or_throw();
    };
};

class ContactRequestResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ContactRequestResponse;

    mutable bool accepted{};
    mutable std::string publicIdentityKey;

public:
    ContactRequestResponse() = default;
    ContactRequestResponse(bool accepted_, std::string publicIdentityKey_) : accepted(accepted_), publicIdentityKey(std::move(publicIdentityKey_)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(accepted, publicIdentityKey).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->accepted, this->publicIdentityKey).or_throw();
    };
};

class UserProfileRequest final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::UserProfileRequest;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};

class UserProfileRequestResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::UserProfileRequestResponse;

    mutable UserProfile userProfile;

public:
    UserProfileRequestResponse() = default;
    explicit UserProfileRequestResponse(UserProfile  userProfile) : userProfile(std::move(userProfile)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->userProfile).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->userProfile).or_throw();
    };
};

class PublicKeyRequest final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::PublicKeyRequest;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};

class PublicKeyRequestResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::PublicKeyRequestResponse;

    mutable std::string publicKey;

public:
    PublicKeyRequestResponse() = default;
    explicit PublicKeyRequestResponse(std::string publicKey_) : publicKey(std::move(publicKey_)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->publicKey).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->publicKey).or_throw();
    };
};

class ChannelRequest final : public BaseMessage<MessageType>
{
public:
    struct ChannelMember
    {
        std::string fingerprint;
        std::vector<std::string> roles;
    };

    struct ChannelConfig
    {
        int16_t maxMembers{};
    };

    struct Role
    {
        std::string name;
        std::vector<int32_t> permissions;
    };

public:
    static const constexpr MessageType opcode = MessageType::ChannelRequest;

    mutable std::string channelUuid;
    mutable std::string channelName;
    mutable std::string channelDescription;
    mutable ChannelConfig channelConfig;
    mutable std::vector<ChannelMember> presentUsers;
    mutable std::vector<Role> roles;
    mutable std::string userRole;

public:
    ChannelRequest() = default;
    explicit ChannelRequest(std::string channelUuid, std::string channelName, std::string channelDescription, ChannelConfig channelConfig, std::vector<ChannelMember> presentUsers, std::vector<Role> roles, std::string userRole)
        : channelUuid(std::move(channelUuid)), channelName(std::move(channelName)), channelDescription(std::move(channelDescription)), channelConfig(std::move(channelConfig)), presentUsers(std::move(presentUsers)), roles(std::move(roles)), userRole(userRole) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->channelUuid, this->channelName, this->channelDescription, this->channelConfig, this->presentUsers, this->roles, this->userRole).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->channelUuid, this->channelName, this->channelDescription, this->channelConfig, this->presentUsers, this->roles, this->userRole).or_throw();
    };
};

class ChannelRequestResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ChannelRequestResponse;

    mutable std::string channelUuid;
    mutable bool accepted;

public:
    ChannelRequestResponse() = default;
    explicit ChannelRequestResponse(std::string channelUuid, bool accepted) : channelUuid(std::move(channelUuid)), accepted(accepted) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->channelUuid, this->accepted).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->channelUuid, this->accepted).or_throw();
    };
};

class ChannelUserJoin final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ChannelUserJoin;

    mutable std::string channelUuid;
    mutable struct ChannelMember
    {
        std::string fingerprint;
        std::vector<std::string> roles;
    } newChannelMember;

public:
    ChannelUserJoin() = default;
    explicit ChannelUserJoin(std::string channelUuid, ChannelMember newChannelMember) : channelUuid(std::move(channelUuid)), newChannelMember(std::move(newChannelMember)) {};
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->channelUuid, this->newChannelMember).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->channelUuid, this->newChannelMember).or_throw();
    };
};

class ChannelUserLeave final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ChannelUserLeave;

    mutable std::string channelUuid;
    mutable std::string userFingerprint;

public:
    ChannelUserLeave() = default;
    explicit ChannelUserLeave(std::string channelUuid, std::string userFingerprint) : channelUuid(std::move(channelUuid)), userFingerprint(std::move(userFingerprint)) {};
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->channelUuid, this->userFingerprint).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->channelUuid, this->userFingerprint).or_throw();
    };
};

class SignedMessage final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::SignedMessage;

    mutable std::vector<std::byte> message;
    mutable std::string signature;

public:
    SignedMessage() = default;
    explicit SignedMessage(auto message, auto signature) : message(message), signature(signature) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->message, this->signature).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->message, this->signature).or_throw();
    };
};

class SendLocalDescription final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::SendLocalDescription;

    mutable std::string fingerprint;
    mutable std::string type;
    mutable std::string description;

public:
    SendLocalDescription() = default;
    explicit SendLocalDescription(std::string fingerprint, std::string type, std::string description) : fingerprint(std::move(fingerprint)), type(std::move(type)), description(std::move(description)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->fingerprint, this->type, this->description).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->fingerprint, this->type, this->description).or_throw();
    };
};

class SendLocalCandidate final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::SendLocalCandidate;

    mutable std::string fingerprint;
    mutable std::string candidate;
    mutable std::string mid;

    SendLocalCandidate() = default;
    explicit SendLocalCandidate(std::string fingerprint, std::string candidate, std::string mid) : fingerprint(std::move(fingerprint)), candidate(std::move(candidate)), mid(std::move(mid)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->fingerprint, this->candidate, this->mid).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->fingerprint, this->candidate, this->mid).or_throw();
    };
};

class ReceiveRemoteDescription final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ReceiveRemoteDescription;

    mutable std::string fingerprint;
    mutable std::string type;
    mutable std::string description;

    ReceiveRemoteDescription() = default;
    explicit ReceiveRemoteDescription(std::string fingerprint, std::string type, std::string description) : fingerprint(std::move(fingerprint)), type(std::move(type)), description(std::move(description)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->fingerprint, this->type, this->description).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->fingerprint, this->type, this->description).or_throw();
    };
};

class ReceiveRemoteCandidate final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::ReceiveRemoteCandidate;

    mutable std::string fingerprint;
    mutable std::string candidate;
    mutable std::string mid;

    ReceiveRemoteCandidate() = default;
    explicit ReceiveRemoteCandidate(std::string fingerprint, std::string candidate, std::string mid) : fingerprint(std::move(fingerprint)), candidate(std::move(candidate)), mid(std::move(mid)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(this->fingerprint, this->candidate, this->mid).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->fingerprint, this->candidate, this->mid).or_throw();
    };
};

class ResetExchange final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::Reset;

    mutable int16_t exchangeType{};

public:
    ResetExchange() = default;
    explicit ResetExchange(auto exchangeType) : exchangeType(static_cast<int16_t>(exchangeType)) {};

    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override
    {
        auto [data, out] = zpp::bits::data_out();
        out(exchangeType).or_throw();
        return data;
    };
    void Deserialize(const std::vector<std::byte>& data) const override
    {
        zpp::bits::in in(data);
        in(this->exchangeType).or_throw();
    };
};
