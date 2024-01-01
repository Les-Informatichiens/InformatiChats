//
// Created by Jean on 8/29/2023.
#pragma once

#include "zpp_bits.h"
#include <chrono>
#include <string>
#include <vector>

struct ChatMessage
{
    static std::vector<std::byte> Serialize(const ChatMessage& chatMessage)
    {
        auto [data, out] = zpp::bits::data_out();
        out(chatMessage).or_throw();
        return data;
    }

    static ChatMessage Deserialize(const std::vector<std::byte>& serializedData)
    {
        zpp::bits::in in(serializedData);
        ChatMessage chatMessage;
        in(chatMessage).or_throw();
        return chatMessage;
    }

    std::string content;
    uint64_t timestamp{};
};
