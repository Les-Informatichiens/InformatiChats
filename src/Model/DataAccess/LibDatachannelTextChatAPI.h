//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "ITextChatAPI.h"
#include "LibDatachannelState.h"

#include <rtc/datachannel.hpp>

#include <Model/Models/ChatEntry.h>
#include <Model/Models/ChatMessage.h>

#include <unordered_map>

class LibDatachannelTextChatAPI : ITextChatAPI
{
public:
    explicit LibDatachannelTextChatAPI(LibDatachannelState& state);

    void SendMessageToPeer(const std::string& peerId, const std::string& message) override;

    void InitiateTextChat(const std::string& peerId) override;
    void CloseTextChat(const std::string& peerId) override;
    void OnChatMessage(std::function<void(ChatEntry)> callback) override;

private:
    LibDatachannelState& state;

    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    std::function<void(ChatEntry)> onMessageReceivedCallback;
};
