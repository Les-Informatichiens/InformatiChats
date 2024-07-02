//
// Created by Jonathan Richard on 2024-01-06.
//

#pragma once

#include "Model/Models/Channel.h"

#include <memory>
#include <string>
#include <unordered_map>


class ChannelMap
{
public:
    ChannelMap() = default;

    void AddChannel(const std::shared_ptr<Channel>& channel);
    void RemoveChannel(const std::string& channelUuid);


    [[nodiscard]] bool ContainsChannel(const std::string& channelUuid) const;
    [[nodiscard]] std::shared_ptr<Channel> GetChannel(const std::string& channelUuid) const;
    [[nodiscard]] std::vector<std::shared_ptr<Channel>> GetChannels() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Channel>> channelMap;
};


