//
// Created by Jonathan Richard on 2023-09-17.
//

#include "LibDatachannelPeeringAPI.h"
#include "LibDatachannelAPIEvents.h"
#include "nlohmann/json.hpp"

LibDatachannelPeeringAPI::LibDatachannelPeeringAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    this->state.OnPeerConnectionState([this](std::string peerId, rtc::PeerConnection::State state) {
        std::cout << "State: " << state << std::endl;
        this->onPeerConnectionStateChangeCb(PeerConnectionStateChangeEvent{peerId, static_cast<ConnectionState>(state)});
        if (state == rtc::PeerConnection::State::Closed ||
            state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed)
        {
            if (auto pc = this->state.GetPeerConnection(peerId))
            {
                this->state.ClosePeerConnection(peerId);
                // should maybe close the open datachannels too? needs testing
            }
        }
    });

    networkAPIEventBus.Subscribe("PeerRequestEvent", [this](const EventData& e) {
        auto eventData = static_cast<const PeerRequestEvent&>(e);
        bool requestAccepted = this->onPeerRequestCb(eventData.peerId);
        if (requestAccepted)
        {
            auto pc = this->state.CreatePeerConnection(eventData.peerId);
        }
    });
    networkAPIEventBus.Subscribe("RemoteDescriptionEvent", [this](const EventData& e) {
        auto eventData = static_cast<const RemoteDescriptionEvent&>(e);
        if (auto pc = this->state.GetPeerConnection(eventData.peerId))
        {
            pc->setRemoteDescription(eventData.description);
        }
    });
    networkAPIEventBus.Subscribe("RemoteCandidateEvent", [this](const EventData& e) {
        auto eventData = static_cast<const RemoteCandidateEvent&>(e);
        if (auto pc = this->state.GetPeerConnection(eventData.peerId))
        {
            pc->addRemoteCandidate(eventData.candidate);
        }
    });
}

void LibDatachannelPeeringAPI::AttemptToConnectToPeer(const std::string& peerId)
{
    std::string username = this->state.GetSignalingSocket()->path().value();
    if (peerId == username)
    {
        std::cout << "Cannot connect to own id" << std::endl;
        return;
    }
    if (this->state.GetPeerConnection(peerId))
    {
        std::cout << "Already connected with user: " + peerId << std::endl;
        return;
    }

    std::cout << "Offering to " + peerId << std::endl;
    auto pc = this->state.CreatePeerConnection(peerId);
    auto pingDc = pc->createDataChannel("ping");
    pingDc->onOpen([username, peerId, wdc = std::weak_ptr(pingDc)]() {
        std::cout << "DataChannel from " << peerId << " open" << std::endl;
        if (auto dc = wdc.lock())
            dc->send("Ping from " + username);
    });
}

void LibDatachannelPeeringAPI::OnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback)
{
    this->onPeerConnectionStateChangeCb = callback;
}

void LibDatachannelPeeringAPI::OnPeerRequest(std::function<bool(std::string)> callback)
{
    this->onPeerRequestCb = callback;
}