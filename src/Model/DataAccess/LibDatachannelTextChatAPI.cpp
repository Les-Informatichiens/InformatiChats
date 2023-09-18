//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelTextChatAPI.h"
#include "LibDatachannelAPIEvents.h"

LibDatachannelTextChatAPI::LibDatachannelTextChatAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    this->networkAPIEventBus.Subscribe("OnTextChannelEvent", [this](const EventData& e) {
        auto eventData = static_cast<const OnTextChannelEvent&>(e);
        std::cout << "Text data channel from " << eventData.peerId << " received" << std::endl;
        this->RegisterTextChannel(eventData.peerId, eventData.textChannel);
    });
}

void LibDatachannelTextChatAPI::SendMessageToPeer(const std::string& peerId, const std::string& message)
{
    auto dcIt = this->textChannelMap.find(peerId);
    if (dcIt == this->textChannelMap.end() || !dcIt->second->isOpen())
    {
        return;
    }
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
    if (this->textChannelMap.contains(peerId))
    {
        std::cout << "Text chat has already been initiated with \"" << peerId << "\"" << std::endl;
        return;
    }
    auto dc = pc->createDataChannel("text");
    this->RegisterTextChannel(peerId, dc);
}

void LibDatachannelTextChatAPI::CloseTextChat(const std::string& peerId)
{
    auto dcIt = this->textChannelMap.find(peerId);
    if (dcIt == this->textChannelMap.end() || !dcIt->second->isOpen())
    {
        return;
    }
    dcIt->second->close();
    this->textChannelMap.erase(dcIt);
}

void LibDatachannelTextChatAPI::OnChatMessage(std::function<void(ChatMessageInfo)> callback)
{
    this->onMessageReceivedCallback = callback;
}

void LibDatachannelTextChatAPI::RegisterTextChannel(const std::string& peerId, std::shared_ptr<rtc::Channel> tc)
{

    tc->onOpen([peerId, wtc = std::weak_ptr(tc)]() {
        std::cout << "DataChannel from " << peerId << " open" << std::endl;
        if (auto dc = wtc.lock())
        {
            uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            dc->send(ChatMessage::Serialize(ChatMessage{"Hello from other peer", timestamp}));
        }
    });

    tc->onClosed([this, peerId]() {
        const auto& dcIt = this->textChannelMap.find(peerId);
        if (dcIt != this->textChannelMap.end())
        {
            this->textChannelMap.erase(dcIt);
        }
        std::cout << "DataChannel from " << peerId << " closed" << std::endl;
    });

    tc->onMessage([this, peerId](auto data) {
        ChatMessage message = ChatMessage::Deserialize(std::get<rtc::binary>(data));
        this->onMessageReceivedCallback(ChatMessageInfo{std::move(message.content), std::chrono::milliseconds(message.timestamp), peerId});
    });

    tc->onError([peerId](auto error) {
        std::cout << "Datachannel from " << peerId << " has errored: " << error << std::endl;
    });

    this->textChannelMap.emplace(peerId, tc);
}
