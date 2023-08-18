//
// Created by Jonathan Richard on 2023-07-27.
//

#include "Chat.h"

#include "nlohmann/json.hpp"


Chat::Chat(const ConnectionConfig &config)
    : rtcConfig(), signalingServer(config.signalingServer), signalingServerPort(config.signalingServerPort),
      webSocket(std::make_shared<rtc::WebSocket>()), connected(false)
{
    std::string stunServer;
    if (config.stunServer.empty())
    {
        std::cout
                << "No STUN server is configured. Only local hosts and public IP addresses supported."
                << std::endl;
    }
    else
    {
        if (config.stunServer.substr(0, 5) != "stun:")
        {
            stunServer = "stun:";
        }
        stunServer += config.stunServer + ":" + config.stunServerPort;
        std::cout << "STUN server is " << stunServer << std::endl;
        rtcConfig.iceServers.emplace_back(stunServer);
    }

    //    if (params.udpMux()) {
    //        std::cout << "ICE UDP mux enabled" << std::endl;
    //        config.enableIceUdpMux = true;
    //    }
}


void Chat::CreateDataChannel(std::shared_ptr<rtc::PeerConnection> &pc, const std::string &peerId)
{
    // We are the offerer, so create a data channel to initiate the process
    const std::string label = "test";
    std::cout << "Creating DataChannel with label \"" << label << "\"" << std::endl;
    auto dc = pc->createDataChannel(label);

    RegisterDataChannel(dc, peerId);
}

void Chat::RegisterDataChannel(const std::shared_ptr<rtc::DataChannel> &dc, const std::string &peerId)
{
    dc->onOpen([this, peerId, wdc = std::weak_ptr(dc)]() {
        std::cout << "DataChannel from " << peerId << " open" << std::endl;
        if (auto dc = wdc.lock())
            dc->send("Hello from " + username);
    });

    dc->onClosed([this, peerId]() {
        auto dcIt = dataChannelMap.find(peerId);
        if (dcIt != dataChannelMap.end())
        {
            dataChannelMap.erase(peerId);
        }

        // TODO: find out why does this crash?
        //        auto pcIt = peerConnectionMap.find(peerId);
        //        if (pcIt != peerConnectionMap.end())
        //        {
        //            pcIt->second->close();
        //        }

        std::cout << "DataChannel from " << peerId << " closed" << std::endl;
    });

    dc->onMessage([this, peerId](auto data) {
        // data holds either std::string or rtc::binary
        if (std::holds_alternative<std::string>(data))
        {
            onMessageReceivedCallback(MessageReceivedEvent{peerId, std::get<std::string>(data)});
            std::cout << "Message from " << peerId << " received: " << std::get<std::string>(data)
                      << std::endl;
        }
        else
        {
            std::cout << "Binary message from " << peerId
                      << " received, size=" << std::get<rtc::binary>(data).size() << std::endl;
        }
    });

    dc->onError([peerId](auto error) {
        std::cout << "Datachannel from " << peerId << " has errored: " << error << std::endl;
    });

    dataChannelMap.emplace(peerId, dc);
}

void Chat::SendMessageToPeer(const std::string &peerId, const char *message)
{
    auto dcIt = dataChannelMap.find(peerId);
    if (dcIt == dataChannelMap.end())
        return;
    dcIt->second->send(message);
}

std::shared_ptr<rtc::PeerConnection> Chat::CreatePeerConnection(const std::string &peerId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(rtcConfig);

    pc->onStateChange([this, peerId, pc](rtc::PeerConnection::State state) {
        std::cout << "State: " << state << std::endl;
        if (state == rtc::PeerConnection::State::Closed ||
            state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed)
        {
            if (peerConnectionMap.find(peerId) != peerConnectionMap.end())
            {
                peerConnectionMap.erase(peerId);
                // should maybe close the open datachannels too? needs testing
            }
        }
    });

    pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state) {
        std::cout << "Gathering State: " << state << std::endl;
    });

    pc->onLocalDescription([wss = std::weak_ptr(webSocket), peerId](const rtc::Description &description) {
        nlohmann::json message = {{"id", peerId},
                                  {"type", description.typeString()},
                                  {"description", std::string(description)}};
        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onLocalCandidate([wss = std::weak_ptr(webSocket), peerId](const rtc::Candidate &candidate) {
        nlohmann::json message = {{"id", peerId},
                                  {"type", "candidate"},
                                  {"candidate", std::string(candidate)},
                                  {"mid", candidate.mid()}};

        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onDataChannel([peerId, this](const std::shared_ptr<rtc::DataChannel> &dc) {
        std::cout << "DataChannel from " << peerId << " received with label \"" << dc->label() << "\""
                  << std::endl;

        RegisterDataChannel(dc, peerId);
    });

    peerConnectionMap.emplace(peerId, pc);
    return pc;
}
