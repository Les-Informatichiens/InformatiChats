//
// Created by Jonathan Richard on 2023-09-17.
//

#include "LibDatachannelPeeringAPI.h"

#include "LibDatachannelAPIEvents.h"
#include "nlohmann/json.hpp"
#include "zpp_bits.h"
#include <utility>

LibDatachannelPeeringAPI::LibDatachannelPeeringAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    networkAPIEventBus.Subscribe("PeerRequestEvent", [this](const EventData& e) {
        auto eventData = static_cast<const PeerRequestEvent&>(e);

        bool requestAccepted = this->onPeerRequestCb(eventData.peerId);
        if (requestAccepted)
        {
            auto pc = this->CreatePeerConnection(eventData.peerId);
            if (this->onNewPeerCb)
                this->onNewPeerCb(eventData.peerId);
        }
    });
    networkAPIEventBus.Subscribe("ReceiveRemoteDescriptionEvent", [this](const EventData& e) {
        auto eventData = static_cast<const ReceiveRemoteDescriptionEvent&>(e);
        if (auto peer = this->state.GetPeer(eventData.peerId))
        {
            std::cout << eventData.peerId << " has sent description : " << eventData.description << std::endl;
            peer->SetRemoteDescription(eventData.description);
        }
    });
    networkAPIEventBus.Subscribe("ReceiveRemoteCandidateEvent", [this](const EventData& e) {
        auto eventData = static_cast<const ReceiveRemoteCandidateEvent&>(e);
        if (auto peer = this->state.GetPeer(eventData.peerId))
        {
            std::cout << eventData.peerId << " has sent candidate : " << eventData.candidate << std::endl;
            peer->AddRemoteCandidate(eventData.candidate);
        }
    });
}

void LibDatachannelPeeringAPI::Init(const PeeringConfig& peeringConfig)
{
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

void LibDatachannelPeeringAPI::OpenPeerConnection(const std::string& peerId, std::function<void()> onReady)
{
    /*
    if (auto pc = this->state.GetPeerConnection(peerId))
    {
        if (pc->state() == rtc::PeerConnection::State::Connected)
        {
            return;
        }
    }
     */

    std::cout << "Offering to " + peerId << std::endl;
    auto peer = this->CreatePeerConnection(peerId);
    peer->OnConnected(onReady);
    peer->Connect();
}

void LibDatachannelPeeringAPI::ClosePeerConnection(const std::string& peerId)
{
    this->state.GetPeer(peerId)->Disconnect();
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

void LibDatachannelPeeringAPI::OnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback)
{
    this->onPeerConnectionStateChangeCb = std::move(callback);
}

void LibDatachannelPeeringAPI::OnPeerRequest(std::function<bool(const std::string&)> callback)
{
    this->onPeerRequestCb = std::move(callback);
}

void LibDatachannelPeeringAPI::OnNewPeer(std::function<void(const std::string&)> callback)
{
    this->onNewPeerCb = std::move(callback);
}

std::shared_ptr<Peer> LibDatachannelPeeringAPI::CreatePeerConnection(const std::string& peerId)
{
    auto peer = std::make_shared<Peer>(peerId, this->networkAPIEventBus, this->rtcConfig);
    this->state.RegisterPeer(peer);
    peer->OnStateChange([this, peerId](rtc::PeerConnection::State connectionState) {
        if (this->onPeerConnectionStateChangeCb)
            this->onPeerConnectionStateChangeCb(PeerConnectionStateChangeEvent{peerId, static_cast<ConnectionState>(connectionState)});
    });
    this->networkAPIEventBus.Publish(OnNewPeerEvent{peer});
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
    std::cout << "hello" << std::endl;
}