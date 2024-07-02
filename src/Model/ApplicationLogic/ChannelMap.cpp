//
// Created by Jonathan Richard on 2024-01-06.
//

#include "ChannelMap.h"

void ChannelMap::AddChannel(const std::shared_ptr<Channel>& channel)
{
    this->channelMap.emplace(channel->GetUuid(), channel);
}

void ChannelMap::RemoveChannel(const std::string& channelUuid)
{
    this->channelMap.erase(channelUuid);
}


bool ChannelMap::ContainsChannel(const std::string& channelUuid) const
{
    return this->channelMap.contains(channelUuid);
}

std::shared_ptr<Channel> ChannelMap::GetChannel(const std::string& channelUuid) const
{
    if (const auto channelIt = this->channelMap.find(channelUuid); channelIt != this->channelMap.end())
    {
        return channelIt->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Channel>> ChannelMap::GetChannels() const
{
    std::vector<std::shared_ptr<Channel>> channels;
    for (const auto& channel: this->channelMap | std::views::values)
    {
        channels.push_back(channel);
    }
    return channels;
}
