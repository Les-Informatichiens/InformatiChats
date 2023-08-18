//
// Created by Jean on 8/18/2023.
//

#include "LoginController.h"

void LoginController::AttemptConnectionWithUsername(const std::string &newUsername)
{
    auto wsFuture = wsPromise.get_future();

    webSocket->onOpen([this, newUsername]() {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        wsPromise.set_value();

        username = newUsername;
        connected = true;
    });

    webSocket->onError([this](const std::string &s) {
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

        std::cout << "The message sent by the server is: " << message << std::endl;


        auto it = message.find("id");
        if (it == message.end())
            return;

        auto id = it->get<std::string>();

        // If the id in the message returned from the server is the same as the user,
        // it means that the server couldn't find the wanted user.
        if (id == username)
        {
            auto destIt = message.find("destination_id");

            if (destIt == message.end())
            {
                std::cout << "Could not find user, destination ID is invalid" << std::endl;
                return;
            }
            std::cout << "Could not find user: " << destIt->get<std::string>() << ". Operation cancelled." << std::endl;

            auto badPcIt = peerConnectionMap.find(destIt->get<std::string>());
            if (badPcIt != peerConnectionMap.end())
            {
                badPcIt->second->close();
            }
            return;
        }

        it = message.find("type");
        if (it == message.end())
            return;

        auto type = it->get<std::string>();

        std::shared_ptr<rtc::PeerConnection> pc;
        if (auto jt = peerConnectionMap.find(id); jt != peerConnectionMap.end())
        {
            pc = jt->second;
        }
        else if (type == "offer")
        {
            std::cout << "Answering to " + id << std::endl;
            pc = CreatePeerConnection(id);
        }
        else
        {
            std::cout << "Could not establish connection to " << id << std::endl;
            return;
        }

        if (type == "offer" || type == "answer")
        {
            auto sdp = message["description"].get<std::string>();
            pc->setRemoteDescription(rtc::Description(sdp, type));
        }
        else if (type == "candidate")
        {
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

std::shared_ptr<rtc::PeerConnection> LoginController::CreatePeerConnection(const std::string &peerId)
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

void LoginController::RegisterDataChannel(const std::shared_ptr<rtc::DataChannel> &dc, const std::string &peerId)
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

LoginViewModel LoginController::getViewModel()
{
    return {maxNameLength};
}
