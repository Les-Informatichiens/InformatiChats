//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelConnectionAPI.h"

LibDatachannelConnectionAPI::LibDatachannelConnectionAPI(LibDatachannelState& state)
    : state(state)
{
}

void LibDatachannelConnectionAPI::ConnectWithUsername(const std::string& username)
{
    auto wsFuture = this->wsPromise.get_future();

    this->webSocket->onOpen([this, username]() {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        this->wsPromise.set_value();

        this->username = username;
        this->connected = true;
    });

    this->webSocket->onError([this](const std::string& s) {
        std::cout << "WebSocket error" << std::endl;
        this->wsPromise.set_exception(std::make_exception_ptr(std::runtime_error(s)));
    });

    this->webSocket->onClosed([this]() {
        std::cout << "WebSocket closed" << std::endl;

        connected = false;
    });

    this->webSocket->onMessage([wws = std::weak_ptr(this->webSocket), this](auto data) {
        // data holds either std::string or rtc::binary
        if (!std::holds_alternative<std::string>(data))
            return;

        nlohmann::json message = nlohmann::json::parse(std::get<std::string>(data));

        std::cout << "The message sent by the server is: " << message << std::endl;


        auto it = message.find("id");
        if (it == message.end())
            return;

        auto id = it->get<std::string>();


        // Username should never be empty
        if (id.empty())
        {
            std::cout << "User with empty name tried to open a connection. Operation cancelled." << std::endl;
            return;
        }

        // If the id in the message returned from the server is the same as the user,
        // it means that the server couldn't find the wanted user.
        if (id == this->username)
        {
            auto destIt = message.find("destination_id");

            if (destIt == message.end())
            {
                std::cout << "Could not find user, destination ID is invalid" << std::endl;
                return;
            }
            std::cout << "Could not find user: " << destIt->get<std::string>() << ". Operation cancelled." << std::endl;

            if (auto badPc = this->state.GetPeerConnection(destIt->get<std::string>()))
            {
                badPc->close();
            }
            return;
        }

        it = message.find("type");
        if (it == message.end())
            return;

        auto type = it->get<std::string>();

        std::shared_ptr<rtc::PeerConnection> pc;

        if (auto jt = this->state.GetPeerConnection(id))
        {
            pc = jt;
        }
        else if (type == "offer")
        {
            std::cout << "Answering to " + id << std::endl;
            pc = this->CreatePeerConnection(id);
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

    const std::string wsPrefix = "wss://";
    const std::string url = wsPrefix + this->signalingServer + ":" +
                            this->signalingServerPort + "/" + username;

    std::cout << "WebSocket URL is " << url << std::endl;
    this->webSocket->open(url);

    std::cout << "Waiting for signaling to be connected..." << std::endl;
    wsFuture.get();
}

void LibDatachannelConnectionAPI::AttemptToConnectToPeer(const std::string& peerId)
{
    if (peerId == this->username)
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
    this->state.CreatePeerConnection(peerId);
}

std::shared_ptr<rtc::PeerConnection> LibDatachannelConnectionAPI::CreatePeerConnection(const std::string& peerId)
{
    auto pc = this->state.CreatePeerConnection(peerId);
    pc->onLocalDescription([wss = std::weak_ptr(this->webSocket), peerId](const rtc::Description& description) {
        nlohmann::json message = {{"id", peerId},
                                  {"type", description.typeString()},
                                  {"description", std::string(description)}};
        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onLocalCandidate([wss = std::weak_ptr(this->webSocket), peerId](const rtc::Candidate& candidate) {
        nlohmann::json message = {{"id", peerId},
                                  {"type", "candidate"},
                                  {"candidate", std::string(candidate)},
                                  {"mid", candidate.mid()}};

        if (auto ws = wss.lock())
            ws->send(message.dump());
    });
    return pc;
}
