//
// Created by Jonathan Richard on 2023-09-17.
//

#include "LibDatachannelPeeringAPI.h"

#include "LibDatachannelAPIEvents.h"
#include "nlohmann/json.hpp"
#include "util/crypto/UUID.h"
#include "zpp_bits.h"

#include <utility>

LibDatachannelPeeringAPI::LibDatachannelPeeringAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    networkAPIEventBus.Subscribe("PeerRequestEvent", [this](const EventData& e) {
        auto eventData = static_cast<const PeerRequestEvent&>(e);

        bool requestAccepted = this->onPeerRequestCb(eventData.peerId, eventData.fingerprint);
        if (requestAccepted)
        {
            const std::string peerId = UUID::generateUUID();
            auto pc = this->CreatePeerConnection(peerId, eventData.peerId);
            if (this->onNewPeerCb)
                this->onNewPeerCb(peerId, eventData.peerId, eventData.fingerprint);
        }
    });
    networkAPIEventBus.Subscribe("ReceiveRemoteDescriptionEvent", [this](const EventData& e) {
        auto eventData = static_cast<const ReceiveRemoteDescriptionEvent&>(e);
        if (auto peer = this->state.GetPeerBySignalingId(eventData.peerId))
        {
            std::cout << eventData.peerId << " has sent description : " << eventData.description << std::endl;
            peer->SetRemoteDescription(eventData.description);
        }
    });
    networkAPIEventBus.Subscribe("ReceiveRemoteCandidateEvent", [this](const EventData& e) {
        auto eventData = static_cast<const ReceiveRemoteCandidateEvent&>(e);
        if (auto peer =  this->state.GetPeerBySignalingId(eventData.peerId))
        {
            std::cout << eventData.peerId << " has sent candidate : " << eventData.candidate << std::endl;
            peer->AddRemoteCandidate(eventData.candidate);
        }
    });
    // networkAPIEventBus.Subscribe("SendLocalDescriptionByPeerEvent", [this](const EventData& e) {
    //     auto eventData = static_cast<const SendLocalDescriptionByPeerEvent&>(e);
    //     if (auto peer = this->state.GetPeer(eventData.intermediatePeerId))
    //     {
    //         peer->SendMessage(SendLocalDescription{eventData.id, eventData.type, eventData.description});
    //     }
    // });
    // networkAPIEventBus.Subscribe("SendLocalCandidateByPeerEvent", [this](const EventData& e) {
    //     auto eventData = static_cast<const SendLocalCandidateByPeerEvent&>(e);
    //     if (auto peer =  this->state.GetPeer(eventData.intermediatePeerId))
    //     {
    //         peer->SendMessage(SendLocalCandidate{eventData.id, eventData.candidate, eventData.mid});
    //     }
    // });
}

void LibDatachannelPeeringAPI::Init(const PeeringConfig& peeringConfig)
{
    this->userFingerprint = peeringConfig.userFingerprint;

    rtc::Configuration config;

    std::string stunServer;
    if (peeringConfig.stunServer.empty())
    {
        std::cout
                << "No STUN server is configured. Only local hosts and public IP addresses supported."
                << std::endl;
    }
    else
    {
        if (peeringConfig.stunServer.substr(0, 5) != "stun:")
        {
            stunServer = "stun:";
        }
        stunServer += peeringConfig.stunServer + ":" + peeringConfig.stunServerPort;
        std::cout << "STUN server is " << stunServer << std::endl;
        config.iceServers.emplace_back(stunServer);
    }
    this->rtcConfig = config;
}

std::string LibDatachannelPeeringAPI::OpenPeerConnection(const std::string& signalingId, std::function<void()> onReady)
{
    std::cout << "Offering to " + signalingId << std::endl;

    std::string peerId = UUID::generateUUID();
    const auto peer = this->CreatePeerConnection(peerId, signalingId);
    peer->OnConnected(onReady);
    peer->Connect();
    return peerId;
}

void LibDatachannelPeeringAPI::ClosePeerConnection(const std::string& peerId)
{
    // auto peer = this->state.GetPeer(peerId);
    // if (peer)
    // {
    //     peer->Disconnect();
    // }
    this->state.DestroyPeer(peerId);
}

void LibDatachannelPeeringAPI::SendMessage(const std::string& peerId, const BaseMessage<MessageType>& message)
{
    auto peer = this->state.GetPeer(peerId);
    if (peer)
    {
        peer->SendMessage(message);
    }
}

std::optional<std::string> LibDatachannelPeeringAPI::GetPeerIpAddress(const std::string& peerId)
{
    if (const auto peer = this->state.GetPeer(peerId))
    {
        return peer->GetIpAddress();
    }
    return std::nullopt;
}

void LibDatachannelPeeringAPI::OnPeerConnectionStateChange(const std::string& peerId, std::function<void(ConnectionState)> callback)
{
    auto peer = this->state.GetPeer(peerId);
    if (peer)
    {
        peer->OnStateChange([callback](rtc::PeerConnection::State connectionState) {
            callback(static_cast<ConnectionState>(connectionState));
        });
    }
    // this->onPeerConnectionStateChangeCb = std::move(callback);
}

void LibDatachannelPeeringAPI::OnPeerRequest(std::function<bool(const std::string&, const std::string&)> callback)
{
    this->onPeerRequestCb = std::move(callback);
}

void LibDatachannelPeeringAPI::OnNewPeer(std::function<void(const std::string&, const std::string&, const std::string&)> callback)
{
    this->onNewPeerCb = std::move(callback);
}

std::shared_ptr<LibDatachannelPeer> LibDatachannelPeeringAPI::CreatePeerConnection(const std::string& peerId, const std::string& signalingId)
{
    auto peer = std::make_shared<LibDatachannelPeer>(peerId, signalingId, this->networkAPIEventBus, this->rtcConfig);
    this->state.RegisterPeer(peer);
    // peer->OnStateChange([this, peerId](rtc::PeerConnection::State connectionState) {
    //     if (this->onPeerConnectionStateChangeCb)
    //         this->onPeerConnectionStateChangeCb(PeerConnectionStateChangeEvent{peerId, static_cast<ConnectionState>(connectionState)});
    // });
    this->networkAPIEventBus.Publish(OnNewPeerEvent{peer});

    peer->SubscribeEvents<TextRequest,
                         TextRequestResponse,
                         TextResponseAck>(
            [](const BaseMessage<MessageType>& message) {
                std::cout << "Message of TYPE " << static_cast<int>(message.GetOpcode()) << " received!" << std::endl;
            });
    return peer;
}

void LibDatachannelPeeringAPI::OnPeerMessage(const std::string& peerId, std::function<void(BaseMessage<MessageType>&)> callback)
{
    auto peer = this->state.GetPeer(peerId);
    if (peer)
    {
        peer->OnMessage(std::move(callback));
    }
}

void LibDatachannelPeeringAPI::OnPeerConnected(const std::string& peerId, std::function<void()> callback)
{
    auto peer = this->state.GetPeer(peerId);
    if (peer)
    {
        peer->OnConnected(std::move(callback));
    }
}
