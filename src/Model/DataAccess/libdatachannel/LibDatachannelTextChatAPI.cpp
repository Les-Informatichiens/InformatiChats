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

LibDatachannelTextChatAPI::~LibDatachannelTextChatAPI()
{
    this->textChannelMap.clear();
}

void LibDatachannelTextChatAPI::SendMessageToPeer(const std::string& peerId, const std::string& destinationChannel, const std::string& message)
{
    // auto dcIt = this->textChannelMap.find(peerId);
    // if (dcIt == this->textChannelMap.end() || !dcIt->second->isOpen())
    // {
    //     return;
    // }
    // const uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // dcIt->second->send(ChatMessage::Serialize(ChatMessage{message, timestamp}));
    auto peer = this->state.GetPeer(peerId);
    if (!peer)
        return;

    if (!peer->GetTextChannel())
        return;

    const uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    peer->GetTextChannel()->send(ChatMessage::Serialize(ChatMessage{destinationChannel, message, timestamp}));
}

void LibDatachannelTextChatAPI::InitiateTextChat(const std::string& peerId)
{
    auto peer = this->state.GetPeer(peerId);
    if (!peer)
    {
        std::cout << "Can't initiate a text chat from inexistant peer connection" << std::endl;
        return;
    }

    // auto dcIt = this->textChannelMap.find(peerId);
    // if (dcIt != this->textChannelMap.end() && dcIt->second && dcIt->second->isOpen())
    if (peer->GetTextChannel() && peer->GetTextChannel()->isOpen())
    {
        std::cout << "Text chat has already been initiated with \"" << peerId << "\"" << std::endl;
        return;
    }

    if (peer->IsConnected())
    {
        std::cout << "Initiating text chat" << std::endl;
        auto textDc = peer->CreateNegotiatedChannel("text", 42);
        this->RegisterTextChannel(peer->GetId(), textDc);
        peer->SetTextChannel(textDc);
    }
}

void LibDatachannelTextChatAPI::CloseTextChat(const std::string& peerId)
{
    // auto dcIt = this->textChannelMap.find(peerId);
    // if (dcIt == this->textChannelMap.end() || !dcIt->second->isOpen())
    // {
    //     return;
    // }
    // dcIt->second->close();


    auto peer = this->state.GetPeer(peerId);
    if (!peer)
        return;

    if (peer->GetTextChannel())
    {
        peer->GetTextChannel()->close();
        peer->SetTextChannel(nullptr);
    }
}

void LibDatachannelTextChatAPI::OnChatMessage(std::function<void(ChatMessageInfo)> callback)
{
    this->onMessageReceivedCallback = callback;
}

void LibDatachannelTextChatAPI::OnChatMessage(const std::string& peerId, std::function<void(ChatMessage)> callback)
{
    auto peer = this->state.GetPeer(peerId);
    if (!peer)
        return;

    peer->OnTextDcMessage([callback](auto data) {
        ChatMessage message = ChatMessage::Deserialize(std::get<rtc::binary>(data));
        callback(message);
    });
}

void LibDatachannelTextChatAPI::RegisterTextChannel(const std::string& peerId, const std::shared_ptr<rtc::Channel>& tc)
{

    tc->onOpen([peerId]() {
        std::cout << "Text channel from " << peerId << " open" << std::endl;
    });

    tc->onClosed([peerId]() {
        std::cout << "Text channel from " << peerId << " closed" << std::endl;
    });

    // tc->onMessage([this, peerId](auto data) {
    //     ChatMessage message = ChatMessage::Deserialize(std::get<rtc::binary>(data));
    //     this->onMessageReceivedCallback(ChatMessageInfo{std::move(message.content), std::chrono::milliseconds(message.timestamp), peerId, {}});
    // });

    tc->onError([peerId](auto error) {
        std::cout << "Datachannel from " << peerId << " has errored: " << error << std::endl;
    });

    // this->textChannelMap.insert_or_assign(peerId, tc);
}