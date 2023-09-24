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
    this->networkAPIEventBus.Subscribe("OnNewPeerEvent", [this](const EventData& e) {
        auto eventData = static_cast<const OnNewPeerEvent&>(e);
        eventData.peer->SubscribeEvent<TextRequest>([this, wpeer = std::weak_ptr(eventData.peer)](const auto& request) {
            auto peer = wpeer.lock();
            if (!peer)
                return;

            std::cout << "Received text request to " + peer->GetId() + ", sending response." << std::endl;

            auto textDc = peer->CreateNegotiatedChannel("text", 42);
            this->RegisterTextChannel(peer->GetId(), textDc);
            peer->Send(TextResponse{});
        });
        eventData.peer->SubscribeEvent<TextResponse>([this, wpeer = std::weak_ptr(eventData.peer)](const auto& response) {
            auto peer = wpeer.lock();
            if (!peer)
                return;

            std::cout << "Received text approval from " + peer->GetId() << std::endl;
            auto textDc = peer->CreateNegotiatedChannel("text", 42);
            this->RegisterTextChannel(peer->GetId(), textDc);
        });
    });
}

LibDatachannelTextChatAPI::~LibDatachannelTextChatAPI()
{
    this->textChannelMap.clear();
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
    auto peer = this->state.GetPeer(peerId);
    if (!peer)
    {
        std::cout << "Can't initiate a text chat from inexistant peer connection" << std::endl;
        return;
    }

    auto dcIt = this->textChannelMap.find(peerId);
    if (dcIt != this->textChannelMap.end() && dcIt->second && dcIt->second->isOpen())
    {
        std::cout << "Text chat has already been initiated with \"" << peerId << "\"" << std::endl;
        return;
    }

    if (peer->IsConnected())
    {
        std::cout << "Requesting text to " + peerId << std::endl;
        peer->Send(TextRequest{});
        //        auto [data, out] = zpp::bits::data_out();
        //        out(0).or_throw();
        //        peer->dc->send(data);
        //        peer.dc->send()
//        auto negDc = peer.pc->createDataChannel("text", init);
    }
//    auto dc = pc->createDataChannel("text");
//    this->RegisterTextChannel(peerId, dc);
}

void LibDatachannelTextChatAPI::CloseTextChat(const std::string& peerId)
{
    auto dcIt = this->textChannelMap.find(peerId);
    if (dcIt == this->textChannelMap.end() || !dcIt->second->isOpen())
    {
        return;
    }
    dcIt->second->close();
}

void LibDatachannelTextChatAPI::OnChatMessage(std::function<void(ChatMessageInfo)> callback)
{
    this->onMessageReceivedCallback = callback;
}

void LibDatachannelTextChatAPI::RegisterTextChannel(const std::string& peerId, const std::shared_ptr<rtc::Channel>& tc)
{

    tc->onOpen([peerId, wtc = std::weak_ptr(tc)]() {
        std::cout << "Text channel from " << peerId << " open" << std::endl;
//        if (auto dc = wtc.lock())
//        {
//            uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//            dc->send(ChatMessage::Serialize(ChatMessage{"Hello from other peer", timestamp}));
//        }
    });

    tc->onClosed([this, peerId]() {
        const auto& dcIt = this->textChannelMap.find(peerId);
        std::cout << "Text channel from " << peerId << " closed" << std::endl;
        //        if (dcIt != this->textChannelMap.end())
        //        {
        //            dcIt->second.reset();
        //        }
    });

    tc->onMessage([this, peerId](auto data) {
        ChatMessage message = ChatMessage::Deserialize(std::get<rtc::binary>(data));
        this->onMessageReceivedCallback(ChatMessageInfo{std::move(message.content), std::chrono::milliseconds(message.timestamp), peerId});
    });

    tc->onError([peerId](auto error) {
        std::cout << "Datachannel from " << peerId << " has errored: " << error << std::endl;
    });

    this->textChannelMap.insert_or_assign(peerId, tc);
}