#include "LibDataChannelChatAPI.h"

bool LibDataChannelChatAPI::ICEServerExists() const
{
    return this->rtcConfig.iceServers.capacity() > 0;
}

void LibDataChannelChatAPI::AttemptConnectionWithUsername(const std::string& newUsername)
{
    auto wsFuture = this->wsPromise.get_future();

    this->webSocket->onOpen([this, newUsername]() {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        this->wsPromise.set_value();

        this->username = newUsername;
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

            auto badPcIt = this->peerConnectionMap.find(destIt->get<std::string>());
            if (badPcIt != this->peerConnectionMap.end())
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
        if (auto jt = this->peerConnectionMap.find(id); jt != this->peerConnectionMap.end())
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

    const std::string wsPrefix = "wss://";
    const std::string url = wsPrefix + this->signalingServer + ":" +
                            this->signalingServerPort + "/" + newUsername;

    std::cout << "WebSocket URL is " << url << std::endl;
    this->webSocket->open(url);

    std::cout << "Waiting for signaling to be connected..." << std::endl;
    wsFuture.get();
}

void LibDataChannelChatAPI::CreateDataChannel(std::shared_ptr<rtc::PeerConnection>& pc, const std::string& peerId)
{
    // We are the offerer, so create a data channel to initiate the process
    const std::string label = "test";
    std::cout << "Creating DataChannel with label \"" << label << "\"" << std::endl;
    auto dc = pc->createDataChannel(label);

    RegisterDataChannel(dc, peerId);
}

void LibDataChannelChatAPI::AttemptToConnectToPeer(const std::string& peerId)
{
    if (peerId == this->username)
    {
        std::cout << "Cannot connect to own id" << std::endl;
        return;
    }
    if (this->peerConnectionMap.find(peerId) != this->peerConnectionMap.end())
    {
        std::cout << "Already connected with user: " + peerId << std::endl;
        return;
    }

    std::cout << "Offering to " + peerId << std::endl;
    auto pc = CreatePeerConnection(peerId);
    CreateDataChannel(pc, peerId);
}

void LibDataChannelChatAPI::Init(const ConnectionConfig& config_)
{
    std::string stunServer;
    if (config_.stunServer.empty())
    {
        std::cout
                << "No STUN server is configured. Only local hosts and public IP addresses supported."
                << std::endl;
    }
    else
    {
        if (config_.stunServer.substr(0, 5) != "stun:")
        {
            stunServer = "stun:";
        }
        stunServer += config_.stunServer + ":" + config_.stunServerPort;
        std::cout << "STUN server is " << stunServer << std::endl;
        this->rtcConfig.iceServers.emplace_back(stunServer);
    }

    this->signalingServer = config_.signalingServer;
    this->signalingServerPort = config_.signalingServerPort;

    this->webSocket = std::make_shared<rtc::WebSocket>();
}

std::shared_ptr<rtc::PeerConnection> LibDataChannelChatAPI::CreatePeerConnection(const std::string& peerId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(this->rtcConfig);

    pc->onStateChange([this, peerId, pc](rtc::PeerConnection::State state) {
        std::cout << "State: " << state << std::endl;
        this->onPeerConnectionStateChangeCallback(PeerConnectionStateChangeEvent{peerId, static_cast<ConnectionState>(state)});
        if (state == rtc::PeerConnection::State::Closed ||
            state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed)
        {
            if (this->peerConnectionMap.find(peerId) != this->peerConnectionMap.end())
            {
                this->peerConnectionMap.erase(peerId);
                // should maybe close the open datachannels too? needs testing
            }
        }
    });

    pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state) {
        std::cout << "Gathering State: " << state << std::endl;
    });

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

    pc->onDataChannel([peerId, this](const std::shared_ptr<rtc::DataChannel>& dc) {
        std::cout << "DataChannel from " << peerId << " received with label \"" << dc->label() << "\""
                  << std::endl;

        RegisterDataChannel(dc, peerId);
    });

    this->peerConnectionMap.emplace(peerId, pc);
    return pc;
}

void LibDataChannelChatAPI::RegisterDataChannel(const std::shared_ptr<rtc::DataChannel>& dc, const std::string& peerId)
{
    dc->onOpen([this, peerId, wdc = std::weak_ptr(dc)]() {
        std::cout << "DataChannel from " << peerId << " open" << std::endl;
        if (auto dc = wdc.lock())
            dc->send("Hello from " + this->username);
    });

    dc->onClosed([this, peerId]() {
        const auto& dcIt = this->dataChannelMap.find(peerId);
        if (dcIt != this->dataChannelMap.end())
        {
            this->dataChannelMap.erase(peerId);
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
            this->onMessageReceivedCallback(ChatMessage{
                    std::get<std::string>(data),
                    duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
                    peerId});
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

    this->dataChannelMap.emplace(peerId, dc);
}

void LibDataChannelChatAPI::SetOnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback)
{
    this->onPeerConnectionStateChangeCallback = callback;
}

void LibDataChannelChatAPI::SetOnMessageReceived(std::function<void(ChatMessage)> callback)
{
    this->onMessageReceivedCallback = callback;
}

void LibDataChannelChatAPI::Reset()
{
    *this = LibDataChannelChatAPI();
}

void const LibDataChannelChatAPI::SendMessageToPeer(const std::string& peerId, const std::string& message)
{
    auto dcIt = this->dataChannelMap.find(peerId);
    if (dcIt == this->dataChannelMap.end() || !dcIt->second->isOpen())
    {
        return;
    }

    dcIt->second->send(message);
}
