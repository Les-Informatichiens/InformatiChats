//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelConnectionAPI.h"
#include "LibDatachannelAPIEvents.h"
#include "Model/Models/Exchanges/SignalingExchanges.h"
#include "util/crypto/Hashing.h"

LibDatachannelConnectionAPI::LibDatachannelConnectionAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
    this->webSocket = std::make_shared<rtc::WebSocket>();

    networkAPIEventBus.Subscribe("SendLocalDescriptionEvent", [wss = std::weak_ptr(this->webSocket)](const EventData& e) {
        auto eventData = static_cast<const SendLocalDescriptionEvent&>(e);

        nlohmann::json message = {{"client_id", eventData.id},
                                  {"type", eventData.type},
                                  {"description", eventData.description}};
        if (auto ws = wss.lock())
            ws->send(nlohmann::json{{"type", eventData.type}, {"payload", message}}.dump());
    });
    networkAPIEventBus.Subscribe("SendLocalCandidateEvent", [wss = std::weak_ptr(this->webSocket)](const EventData& e) {
        auto eventData = static_cast<const SendLocalCandidateEvent&>(e);

        nlohmann::json message = {{"client_id", eventData.id},
                                  {"type", "candidate"},
                                  {"candidate", eventData.candidate},
                                  {"mid", eventData.mid}};
        if (auto ws = wss.lock())
            ws->send(nlohmann::json{{"type", "candidate"}, {"payload", message}}.dump());
    });
}

void LibDatachannelConnectionAPI::Init(const ConnectionConfig& config_)
{
    rtc::InitLogger(rtc::LogLevel::Debug);

    this->signalingServer = config_.signalingServer;
    this->signalingServerPort = config_.signalingServerPort;
}

void LibDatachannelConnectionAPI::ConnectWithUsername(const std::string& username_, const std::string& publicKey)
{
    this->wsPromise = {};
    auto wsFuture = this->wsPromise.get_future();

    this->webSocket->onOpen([this, username_, publicKey] {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        this->wsPromise.set_value();

        this->webSocket->send(username_);
        this->webSocket->send(Hashing::GeneratePublicKeyFingerprint(publicKey));
        nlohmann::json payload = {{"discoverable", true}};
        this->webSocket->send(nlohmann::json{{"type", "discover"}, {"payload", payload}}.dump());
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

        std::string messageType = message["type"].get<std::string>();
        nlohmann::json messagePayload = message["payload"];

        std::cout << "Received message of type " << messageType << std::endl;
        if (messageType == "client_id")
        {
            std::string clientId = messagePayload["client_id"].get<std::string>();
            this->clientId = clientId;
            std::cout << "Client ID is " << this->clientId << std::endl;
            return;
        }
        else if (messageType == "offer")
        {
            std::string id = messagePayload["client_id"].get<std::string>();
            std::string fingerprint = messagePayload["sender_public_key"].get<std::string>();
            std::cout << "Answering to " + id << std::endl;
            networkAPIEventBus.Publish(PeerRequestEvent(id, fingerprint));

            auto sdp = messagePayload["description"].get<std::string>();
            networkAPIEventBus.Publish(ReceiveRemoteDescriptionEvent(id, rtc::Description(sdp, "offer")));
        }
        else if (messageType == "answer")
        {
            std::string id = messagePayload["client_id"].get<std::string>();
            auto sdp = messagePayload["description"].get<std::string>();
            networkAPIEventBus.Publish(ReceiveRemoteDescriptionEvent(id, rtc::Description(sdp, "answer")));
        }
        else if (messageType == "candidate")
        {
            std::string id = messagePayload["client_id"].get<std::string>();
            auto sdp = messagePayload["candidate"].get<std::string>();
            auto mid = messagePayload["mid"].get<std::string>();
            networkAPIEventBus.Publish(ReceiveRemoteCandidateEvent(id, rtc::Candidate(sdp, mid)));
        }
        else if (messageType == "error")
        {
            std::cout << "Error: " << messagePayload << std::endl;
        }
        else if (messageType == "query_discoverable_response")
        {
            this->exchangeManager.SetExchangeState(ExchangeType::QueryDiscoverableExchange, QueryDiscoverable::CompletedState::Make(messagePayload["discoverable_users"]));
        }
        else if (messageType == "user_query_response")
        {
            this->exchangeManager.SetExchangeState(ExchangeType::QueryClientsByFingerprintExchange, QueryClientsByFingerprint::CompletedState::Make(messagePayload["users"]));
        }
        else
        {
            std::cout << "Unknown message type: " << messageType << std::endl;
        }


//        std::cout << "The message sent by the server is: " << message << std::endl;

        // auto it = message.find("own_client_id");
        // if (it != message.end())
        // {
        //     this->clientId = it->get<std::string>();
        //     std::cout << "Client ID is " << this->clientId << std::endl;
        //     return;
        // }
        //
        // it = message.find("id");
        // if (it == message.end())
        //     return;
        //
        // auto id = it->get<std::string>();


        // // Username should never be empty
        // if (id.empty())
        // {
        //     std::cout << "User with empty name tried to open a connection. Operation cancelled." << std::endl;
        //     return;
        // }

        // it = message.find("type");
        // if (it == message.end())
        //     return;
        //
        // auto type = it->get<std::string>();
        //
        // if (type == "offer")
        // {
        //     std::cout << "Answering to " + id << std::endl;
        //     networkAPIEventBus.Publish(PeerRequestEvent(id));
        // }
        //
        // if (type == "offer" || type == "answer")
        // {
        //     auto sdp = message["description"].get<std::string>();
        //     networkAPIEventBus.Publish(ReceiveRemoteDescriptionEvent(id, rtc::Description(sdp, type)));
        // }
        // else if (type == "candidate")
        // {
        //     auto sdp = message["candidate"].get<std::string>();
        //     auto mid = message["mid"].get<std::string>();
        //     networkAPIEventBus.Publish(ReceiveRemoteCandidateEvent(id, rtc::Candidate(sdp, mid)));
        // }
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

void LibDatachannelConnectionAPI::QueryClientsByPublicKey(std::string publicKey, std::function<void(nlohmann::json)> callback)
{
    auto [started, exchange] = exchangeManager.StartNewExchange<QueryClientsByFingerprint>();
    if (!started)
        return;

    exchange->addObserver<QueryClientsByFingerprint::CompletedState>([callback](const std::shared_ptr<QueryClientsByFingerprint::CompletedState>& newState) {
        callback(newState->getUserData<nlohmann::json>());
    });

    nlohmann::json payload = {{"fingerprint", publicKey}};
    this->webSocket->send(nlohmann::json{{"type", "user_query"}, {"payload", payload}}.dump());
}

void LibDatachannelConnectionAPI::QueryDiscoverableClients(std::function<void(nlohmann::json)> callback)
{
    auto [started, exchange] = exchangeManager.StartNewExchange<QueryDiscoverable>();
    if (!started)
        return;

    exchange->addObserver<QueryDiscoverable::CompletedState>([callback](const std::shared_ptr<QueryDiscoverable::CompletedState>& newState) {
        callback(newState->getUserData<nlohmann::json>());
    });

    this->webSocket->send(nlohmann::json{{"type", "query_discoverable"}, {"payload", {}}}.dump());
}

