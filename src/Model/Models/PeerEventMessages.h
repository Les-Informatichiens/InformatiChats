//
// Created by Jonathan Richard on 2023-09-24.
//

#pragma once

#include "zpp_bits.h"
#include <Model/MessageDispatcher.h>
#include <Model/Models/UserData.h>

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
