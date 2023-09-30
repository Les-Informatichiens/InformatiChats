//
// Created by Jonathan Richard on 2023-09-24.
//

#pragma once

#include "zpp_bits.h"
#include <Model/MessageDispatcher.h>

enum class MessageType
{
    Reset,
    TextRequest,
    TextRequestResponse,
    TextResponseAck
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

class ResetExchange final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::Reset;

    mutable int16_t exchangeType;

public:
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
