//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelTextChatAPI.h"

LibDatachannelTextChatAPI::LibDatachannelTextChatAPI(LibDatachannelState& state)
    : state(state)
{
    this->state.OnTextDataChannel([this](const std::string& peerId, std::shared_ptr<rtc::DataChannel> dc) {
        std::cout << "Text data channel from " << peerId << " received" << std::endl;

        this->dataChannelMap.emplace(peerId, dc);
    });
}

void LibDatachannelTextChatAPI::SendMessageToPeer(const std::string& peerId, const std::string& message)
{
    auto dcIt = this->dataChannelMap.find(peerId);
    if (dcIt == this->dataChannelMap.end() || !dcIt->second->isOpen())
    {
        return;
    }
    uint64_t timestamp = 69;// calculate timestamp here
    dcIt->second->send(ChatMessage::Serialize(ChatMessage{message, timestamp}));
}

void LibDatachannelTextChatAPI::InitiateTextChat(const std::string& peerId)
{
    auto pc = this->state.GetPeerConnection(peerId);

    if (!pc)
    {
        std::cout << "Can't initiate a text chat from inexistant peer connection" << std::endl;
        return;
    }
    auto dc = pc->createDataChannel("text");

    dc->onOpen([peerId, wdc = std::weak_ptr(dc)]() {
        std::cout << "DataChannel from " << peerId << " open" << std::endl;
        if (auto dc = wdc.lock())
            dc->send("Hello from other peer");
    });

    dc->onClosed([this, peerId]() {
        const auto& dcIt = this->dataChannelMap.find(peerId);
        if (dcIt != this->dataChannelMap.end())
        {
            this->dataChannelMap.erase(peerId);
        }
        std::cout << "DataChannel from " << peerId << " closed" << std::endl;
    });

    dc->onMessage([this, peerId](auto data) {
        ChatMessage message = ChatMessage::Deserialize(std::get<rtc::binary>(data));
        this->onMessageReceivedCallback(ChatEntry{std::move(message.content), std::chrono::milliseconds(std::move(message.timestamp)), peerId});
    });

    dc->onError([peerId](auto error) {
        std::cout << "Datachannel from " << peerId << " has errored: " << error << std::endl;
    });

    this->dataChannelMap.emplace(peerId, dc);
}

void LibDatachannelTextChatAPI::CloseTextChat(const std::string& peerId)
{
    auto dcIt = this->dataChannelMap.find(peerId);
    if (dcIt == this->dataChannelMap.end() || !dcIt->second->isOpen())
    {
        return;
    }
    dcIt->second->close();
    this->dataChannelMap.erase(dcIt);
}

void LibDatachannelTextChatAPI::OnChatMessage(std::function<void(ChatEntry)> callback)
{
    this->onMessageReceivedCallback = callback;
}
