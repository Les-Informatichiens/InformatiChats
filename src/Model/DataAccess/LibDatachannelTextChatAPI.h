//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "ITextChatAPI.h"
#include "LibDatachannelState.h"
#include "Model/EventBus.h"

#include <rtc/datachannel.hpp>

#include <Model/Models/ChatMessage.h>
#include <Model/Models/ChatMessageInfo.h>

#include <unordered_map>

class LibDatachannelTextChatAPI : public ITextChatAPI
{
public:
    explicit LibDatachannelTextChatAPI(LibDatachannelState& state, EventBus& networkAPIEventBus);

    void InitiateTextChat(const std::string& peerId) override;
    void CloseTextChat(const std::string& peerId) override;

    void SendMessageToPeer(const std::string& peerId, const std::string& message) override;

    void OnChatMessage(std::function<void(ChatMessageInfo)> callback) override;

private:
    void RegisterTextChannel(const std::string& peerId, const std::shared_ptr<rtc::Channel>& tc);

private:
    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    std::unordered_map<std::string, std::shared_ptr<rtc::Channel>> textChannelMap;

    std::function<void(ChatMessageInfo)> onMessageReceivedCallback;
};
