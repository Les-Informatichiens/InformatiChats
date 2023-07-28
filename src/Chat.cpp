//
// Created by Jonathan Richard on 2023-07-27.
//

#include "Chat.h"

#include <nlohmann/json.hpp>

Chat::Chat(const ConnectionConfig& config)
    : rtcConfig()
    , signalingServer(config.signalingServer)
    , signalingServerPort(config.signalingServerPort)
    , webSocket(std::make_shared<rtc::WebSocket>())
    , connected(false)
{
    std::string stunServer;
    if (config.stunServer.empty()) {
        std::cout
                << "No STUN server is configured. Only local hosts and public IP addresses supported."
                << std::endl;
    } else {
        if (config.stunServer.substr(0, 5) != "stun:") {
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

void Chat::AttemptConnectionWithUsername(const char *newUsername) {
    auto wsFuture = wsPromise.get_future();

    webSocket->onOpen([this, newUsername]() {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        wsPromise.set_value();

        username = newUsername;
        connected = true;
    });

    webSocket->onError([this](std::string s) {
        std::cout << "WebSocket error" << std::endl;
        wsPromise.set_exception(std::make_exception_ptr(std::runtime_error(s)));
    });

    webSocket->onClosed([this]() {
        std::cout << "WebSocket closed" << std::endl;

        connected = false;
    });

    webSocket->onMessage([wws = std::weak_ptr(webSocket), this](auto data) {
        // data holds either std::string or rtc::binary
        if (!std::holds_alternative<std::string>(data))
            return;

        nlohmann::json message = nlohmann::json::parse(std::get<std::string>(data));

        auto it = message.find("id");
        if (it == message.end())
            return;

        auto id = it->get<std::string>();

        it = message.find("type");
        if (it == message.end())
            return;

        auto type = it->get<std::string>();

        std::shared_ptr<rtc::PeerConnection> pc;
        if (auto jt = peerConnectionMap.find(id); jt != peerConnectionMap.end()) {
            pc = jt->second;
        } else if (type == "offer") {
            std::cout << "Answering to " + id << std::endl;
            pc = CreatePeerConnection(id);
        } else {
            return;
        }

        if (type == "offer" || type == "answer") {
            auto sdp = message["description"].get<std::string>();
            pc->setRemoteDescription(rtc::Description(sdp, type));
        } else if (type == "candidate") {
            auto sdp = message["candidate"].get<std::string>();
            auto mid = message["mid"].get<std::string>();
            pc->addRemoteCandidate(rtc::Candidate(sdp, mid));
        }
    });

    const std::string wsPrefix = "ws://";
    const std::string url = wsPrefix + signalingServer + ":" +
                            signalingServerPort + "/" + newUsername;

    std::cout << "WebSocket URL is " << url << std::endl;
    webSocket->open(url);

    std::cout << "Waiting for signaling to be connected..." << std::endl;
    wsFuture.get();

}

void Chat::SendMessageToPeer(const std::string& peerId, const char *message)
{

}

std::shared_ptr<rtc::PeerConnection> Chat::CreatePeerConnection(std::string userId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(rtcConfig);

    pc->onStateChange(
            [](rtc::PeerConnection::State state) { std::cout << "State: " << state << std::endl; });

    pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state) {
        std::cout << "Gathering State: " << state << std::endl;
    });

    pc->onLocalDescription([wss = std::weak_ptr(webSocket), userId](const rtc::Description& description) {
        nlohmann::json message = {{"id",          userId},
                                  {"type",        description.typeString()},
                                  {"description", std::string(description)}};

        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onLocalCandidate([wss = std::weak_ptr(webSocket), userId](const rtc::Candidate& candidate) {
        nlohmann::json message = {{"id",        userId},
                                  {"type",      "candidate"},
                                  {"candidate", std::string(candidate)},
                                  {"mid",       candidate.mid()}};

        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onDataChannel([userId, this](const std::shared_ptr<rtc::DataChannel>& dc) {
        std::cout << "DataChannel from " << userId << " received with label \"" << dc->label() << "\""
                  << std::endl;

        dc->onOpen([wdc = std::weak_ptr(dc), this]() {
            if (auto dc = wdc.lock())
                dc->send("Hello from " + username);
        });

        dc->onClosed([userId, this]() {
            if (dataChannelMap.find(userId) != dataChannelMap.end())
            {
                dataChannelMap.erase(userId);
            }
            std::cout << "DataChannel from " << userId << " closed" << std::endl;
        });
        dc->onMessage([userId, wdc = std::weak_ptr(dc)](auto data) {
            // data holds either std::string or rtc::binary
            if (std::holds_alternative<std::string>(data))
            {
                std::cout << "Message from " << userId << " received: " << std::get<std::string>(data)
                          << std::endl;
            }
            else
                std::cout << "Binary message from " << userId
                          << " received, size=" << std::get<rtc::binary>(data).size() << std::endl;
        });

        dataChannelMap.emplace(userId, dc);
    });

    peerConnectionMap.emplace(userId, pc);
    return pc;
}