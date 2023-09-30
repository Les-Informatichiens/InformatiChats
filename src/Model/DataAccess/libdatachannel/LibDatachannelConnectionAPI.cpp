//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelConnectionAPI.h"
#include "LibDatachannelAPIEvents.h"

LibDatachannelConnectionAPI::LibDatachannelConnectionAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    this->webSocket = std::make_shared<rtc::WebSocket>();

    networkAPIEventBus.Subscribe("SendLocalDescriptionEvent", [wss = std::weak_ptr(this->webSocket)](const EventData& e) {
        auto eventData = static_cast<const SendLocalDescriptionEvent&>(e);

        nlohmann::json message = {{"id", eventData.id},
                                  {"type", eventData.type},
                                  {"description", eventData.description}};
        if (auto ws = wss.lock())
            ws->send(message.dump());
    });
    networkAPIEventBus.Subscribe("SendLocalCandidateEvent", [wss = std::weak_ptr(this->webSocket)](const EventData& e) {
        auto eventData = static_cast<const SendLocalCandidateEvent&>(e);

        nlohmann::json message = {{"id", eventData.id},
                                  {"type", "candidate"},
                                  {"candidate", eventData.candidate},
                                  {"mid", eventData.mid}};
        if (auto ws = wss.lock())
            ws->send(message.dump());
    });
}

void LibDatachannelConnectionAPI::Init(const ConnectionConfig& config_)
{
    rtc::InitLogger(rtc::LogLevel::Debug);

    this->signalingServer = config_.signalingServer;
    this->signalingServerPort = config_.signalingServerPort;
}

void LibDatachannelConnectionAPI::ConnectWithUsername(const std::string& username_)
{
    this->wsPromise = {};
    auto wsFuture = this->wsPromise.get_future();

    this->webSocket->onOpen([this, username_]() {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        this->wsPromise.set_value();

//        this->username = username_;
        this->connected = true;
    });

    this->webSocket->onError([this](const std::string& s) {
        std::cout << "WebSocket error" << std::endl;
        this->wsPromise.set_exception(std::make_exception_ptr(std::runtime_error(s)));
    });

    this->webSocket->onClosed([this]() {
        std::cout << "WebSocket closed" << std::endl;

        this->connected = false;
    });

    this->webSocket->onMessage([wws = std::weak_ptr(this->webSocket), this, username_](auto data) {
        // data holds either std::string or rtc::binary
        if (!std::holds_alternative<std::string>(data))
            return;

        nlohmann::json message = nlohmann::json::parse(std::get<std::string>(data));

//        std::cout << "The message sent by the server is: " << message << std::endl;


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

        it = message.find("type");
        if (it == message.end())
            return;

        auto type = it->get<std::string>();

        if (type == "offer")
        {
            std::cout << "Answering to " + id << std::endl;
            networkAPIEventBus.Publish(PeerRequestEvent(id));
        }

        if (type == "offer" || type == "answer")
        {
            auto sdp = message["description"].get<std::string>();
            networkAPIEventBus.Publish(ReceiveRemoteDescriptionEvent(id, rtc::Description(sdp, type)));
        }
        else if (type == "candidate")
        {
            auto sdp = message["candidate"].get<std::string>();
            auto mid = message["mid"].get<std::string>();
            networkAPIEventBus.Publish(ReceiveRemoteCandidateEvent(id, rtc::Candidate(sdp, mid)));
        }
    });

    const std::string wsPrefix = "ws://";
    const std::string url = wsPrefix + this->signalingServer + ":" +
                            this->signalingServerPort + "/" + username_;

    std::cout << "WebSocket URL is " << url << std::endl;
    this->webSocket->open(url);

    std::cout << "Waiting for signaling to be connected..." << std::endl;
    wsFuture.get();
}

void LibDatachannelConnectionAPI::Disconnect()
{
    // TODO: impl graceful disconnections
}

bool LibDatachannelConnectionAPI::IsConnected()
{
    return this->connected;
}

void LibDatachannelConnectionAPI::OnConnected(std::function<void()> callback)
{
    this->onConnectedCb = callback;
}