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
        if (auto pc = this->state.GetPeerConnection(eventData.peerId))
        {
            std::cout << eventData.peerId << " has sent description : " << eventData.description << std::endl;
            pc->setRemoteDescription(eventData.description);
        }
    });
    networkAPIEventBus.Subscribe("ReceiveRemoteCandidateEvent", [this](const EventData& e) {
        auto eventData = static_cast<const ReceiveRemoteCandidateEvent&>(e);
        if (auto pc = this->state.GetPeerConnection(eventData.peerId))
        {
            std::cout << eventData.peerId << " has sent candidate : " << eventData.candidate << std::endl;
            pc->addRemoteCandidate(eventData.candidate);
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

void LibDatachannelPeeringAPI::OpenPeerConnection(const std::string& peerId)
{
    // TODO: should this be the task of the API's caller?
//    std::string username = this->state.GetSignalingSocket()->path().value();
//    if (peerId == username)
//    {
//        std::cout << "Cannot connect to own id" << std::endl;
//        return;
//    }
    if (auto pc = this->state.GetPeerConnection(peerId))
    {
        if (pc->state() == rtc::PeerConnection::State::Connected)
        {
            return;
        }
////        this->state.ClosePeerConnection(peerId);
////        std::cout << "Already connected with user: " + peerId << std::endl;
//        auto pingDc = pc->createDataChannel("ping2");
//        pingDc->onOpen([username = std::string("other peer"), peerId, wdc = std::weak_ptr(pingDc)]() {
//            std::cout << "DataChannel from " << peerId << " open" << std::endl;
//            if (auto dc = wdc.lock())
//                dc->send("Ping2 from " + username);
//        });
//        this->yourChannel = pingDc;
//        pc->setLocalDescription();
//        return;
    }

    std::cout << "Offering to " + peerId << std::endl;
    auto pc = this->CreatePeerConnection(peerId);
    auto pingDc = pc->createDataChannel("ping");
    this->RegisterEventChannel(peerId, pingDc);
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

std::shared_ptr<rtc::PeerConnection> LibDatachannelPeeringAPI::CreatePeerConnection(const std::string& peerId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(this->rtcConfig);
    pc->onLocalDescription([this, peerId](const rtc::Description& description) {
        this->networkAPIEventBus.Publish(SendLocalDescriptionEvent(peerId, description.typeString(), description));
    });
    pc->onLocalCandidate([this, peerId](const rtc::Candidate& candidate) {
        this->networkAPIEventBus.Publish(SendLocalCandidateEvent(peerId, std::string(candidate), candidate.mid()));
    });

    pc->onStateChange([this, peerId, wpc = std::weak_ptr(pc)](rtc::PeerConnection::State connectionState) {
        std::cout << "State: " << connectionState << std::endl;
        this->onPeerConnectionStateChangeCb(PeerConnectionStateChangeEvent{peerId, static_cast<ConnectionState>(connectionState)});
        if (connectionState == rtc::PeerConnection::State::Connected)
        {
            //            if (auto pc = wpc.lock())
            //            {
            //                this->state.RegisterPeerConnection(peerId, pc);
            //            }
        }
        // TODO: maybe put this in the onPeerConnectionStateChangeCb implementation by the caller
//        if (connectionState == rtc::PeerConnection::State::Closed ||
//            connectionState == rtc::PeerConnection::State::Failed)
//        {
//            if (auto pc = this->state.GetPeerConnection(peerId))
//            {
//                this->state.ClosePeerConnection(peerId);
//                // should maybe close the open datachannels too? needs testing
//            }
//        }
    });

    pc->onDataChannel([peerId, this](const std::shared_ptr<rtc::DataChannel>& dc) {
        std::cout << "DataChannel from " << peerId << " received with label \"" << dc->label() << "\""
                  << std::endl;

        if (dc->label() == "text")
        {
            this->networkAPIEventBus.Publish(OnTextChannelEvent(peerId, dc));
        }
        else if (dc->label() == "event")
        {
            this->networkAPIEventBus.Publish(OnEventChannelEvent(peerId, dc));
        }
        else
        {
            this->RegisterEventChannel(peerId, dc);
        }
    });
    this->state.RegisterPeerConnection(peerId, pc);
    return pc;
}

void LibDatachannelPeeringAPI::RegisterEventChannel(const std::string& peerId, const std::shared_ptr<rtc::DataChannel>& dc)
{

    dc->onOpen([this, username = std::string("other peer"), peerId, wdc = std::weak_ptr(dc)]() {
        std::cout << "Ping channel open" << std::endl;
//        if (auto dc = wdc.lock())
//            dc->send("Ping from " + username);

        this->onPeerConnectedCb(peerId);
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
}
