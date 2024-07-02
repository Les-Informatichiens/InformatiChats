//
// Created by Jonathan Richard on 2024-01-10.
//

#pragma once

#include <Model/EventBus.h>

struct SetSelectedChannelEvent : public EventData
{
    std::string channelId;

    explicit SetSelectedChannelEvent(std::string channelId) : EventData("SetSelectedChannelEvent"), channelId(std::move(channelId)) {}
};