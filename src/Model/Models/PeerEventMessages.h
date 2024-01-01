//
// Created by Jonathan Richard on 2023-09-24.
//

#pragma once

#include <Model/MessageDispatcher.h>

enum class MessageType
{
    TextRequest,
    TextResponse
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

class TextResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::TextResponse;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};
