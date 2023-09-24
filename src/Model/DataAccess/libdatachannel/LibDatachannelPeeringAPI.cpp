//
// Created by Jonathan Richard on 2023-09-17.
//

#include "LibDatachannelPeeringAPI.h"
#include "LibDatachannelAPIEvents.h"
#include "nlohmann/json.hpp"
#include "zpp_bits.h"

LibDatachannelPeeringAPI::LibDatachannelPeeringAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    networkAPIEventBus.Subscribe("PeerRequestEvent", [this](const EventData& e) {
        auto eventData = static_cast<const PeerRequestEvent&>(e);

        bool requestAccepted = this->onPeerRequestCb(eventData.peerId);
        if (requestAccepted)
        {
            auto pc = this->CreatePeerConnection(eventData.peerId);
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
    std::cout << "Offering to " + peerId << std::endl;
    auto peer = this->CreatePeerConnection(peerId);
    peer->OnConnected(onReady);
    peer->Connect();

    /*
    if (auto pc = this->state.GetPeerConnection(peerId))
    {
        if (pc->state() == rtc::PeerConnection::State::Connected)
        {
            return;
        }
    }

    std::cout << "Offering to " + peerId << std::endl;
    auto pc = this->CreatePeerConnection(peerId);
    auto pingDc = pc->createDataChannel("ping");
    this->RegisterEventChannel(peerId, pingDc, onReady);
     */
}

void LibDatachannelPeeringAPI::ClosePeerConnection(const std::string& peerId)
{
    // TODO: allow closing a connection with a peer. This includes: closing all datachannels and removing the peerConnection from the State.
}

void LibDatachannelPeeringAPI::OnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback)
{
    this->onPeerConnectionStateChangeCb = callback;
}

void LibDatachannelPeeringAPI::OnPeerConnected(std::function<void(std::string)> callback)
{
    this->onPeerConnectedCb = callback;
}

void LibDatachannelPeeringAPI::OnPeerRequest(std::function<bool(std::string)> callback)
{
    this->onPeerRequestCb = callback;
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

void LibDatachannelPeeringAPI::RegisterEventChannel(const std::string& peerId, const std::shared_ptr<rtc::DataChannel>& dc, const std::function<void()>& onReady)
{
    /*
    dc->onOpen([this, username = std::string("other peer"), peerId, wdc = std::weak_ptr(dc), onReady]() {
        std::cout << "Ping channel open" << std::endl;
//        if (auto dc = wdc.lock())
//            dc->send("Ping from " + username);
        if (onReady)
            onReady();
    });
    dc->onMessage([this, peerId, wdc = std::weak_ptr(dc)](auto m) {
//        std::cout << std::get<std::string>(m) << std::endl;
        zpp::bits::in in(std::get<rtc::binary>(m));
        int opcode;
        in(opcode).or_throw();
        if (opcode == 0)
        {
            if (auto dc = wdc.lock())
            {
                Peer peer = this->state.GetPeer(peerId);
                std::cout << "MAX DATACHANNEL STREAM = " << peer.pc->maxDataChannelId() << std::endl;
                rtc::DataChannelInit init;
                init.negotiated = true;
                init.id = 42;
                auto textDc = peer.pc->createDataChannel("text", init);
                this->networkAPIEventBus.Publish(OnTextChannelEvent(peerId, textDc));
                //    auto dc = pc->createDataChannel("text");
                //    this->RegisterTextChannel(peerId, dc);
                std::cout << "Received text request to " + peerId + ", sending response." << std::endl;
                auto [data, out] = zpp::bits::data_out();
                out(1).or_throw();
                dc->send(data);
            }
        }
        else if (opcode == 1)
        {
            Peer peer = this->state.GetPeer(peerId);

            std::cout << "MAX DATACHANNEL STREAM = " << peer.pc->maxDataChannelId() << std::endl;
            rtc::DataChannelInit init;
            init.negotiated = true;
            init.id = 42;
            auto textDc = peer.pc->createDataChannel("test", init);
            std::cout << "Received text approval from " + peerId << std::endl;

            this->networkAPIEventBus.Publish(OnTextChannelEvent(peerId, textDc));
        }
        //          if (auto dc = wdc.lock())
        //              dc->close();
    });
    dc->onClosed([this, peerId] {
        std::cout << "ping channel closed" << std::endl;
    });
    this->state.SetPeerChannel(peerId, dc);
    */
}
