//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "LibDatachannelState.h"
#include "Model/DataAccess/ITextChatAPI.h"
#include "Model/EventBus.h"

#include "rtc/datachannel.hpp"

#include "Model/Models/ChatMessage.h"
#include "Model/Models/ChatMessageInfo.h"

#include <unordered_map>

class LibDatachannelTextChatAPI : public ITextChatAPI
{
public:
    explicit LibDatachannelTextChatAPI(LibDatachannelState& state, EventBus& networkAPIEventBus);

    virtual ~LibDatachannelTextChatAPI();

    void InitiateTextChat(const std::string& peerId) override;
    void CloseTextChat(const std::string& peerId) override;

    void SendMessageToPeer(const std::string& peerId, const std::string& destinationChannel, const std::string& message) override;

    void OnChatMessage(std::function<void(ChatMessageInfo)> callback) override;
    void OnChatMessage(const std::string& peerId, std::function<void(ChatMessage)> callback) override;

private:
    void RegisterTextChannel(const std::string& peerId, const std::shared_ptr<rtc::Channel>& tc);

public:
    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    std::unordered_map<std::string, std::shared_ptr<rtc::Channel>> textChannelMap;

    std::function<void(ChatMessageInfo)> onMessageReceivedCallback;
};
