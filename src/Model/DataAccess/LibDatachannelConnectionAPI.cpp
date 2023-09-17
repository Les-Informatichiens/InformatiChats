//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelConnectionAPI.h"
#include "LibDatachannelAPIEvents.h"

LibDatachannelConnectionAPI::LibDatachannelConnectionAPI(LibDatachannelState& state, EventBus& networkAPIEventBus)
    : state(state), networkAPIEventBus(networkAPIEventBus)
{
}

void LibDatachannelConnectionAPI::ConnectWithUsername(const std::string& username_)
{
    auto webSocket = this->state.GetSignalingSocket();
    auto wsFuture = this->wsPromise.get_future();

    webSocket->onOpen([this, username_]() {
        std::cout << "WebSocket connected, signaling ready" << std::endl;
        this->wsPromise.set_value();

//        this->username = username_;
        this->connected = true;
    });

    webSocket->onError([this](const std::string& s) {
        std::cout << "WebSocket error" << std::endl;
        this->wsPromise.set_exception(std::make_exception_ptr(std::runtime_error(s)));
    });

    webSocket->onClosed([this]() {
        std::cout << "WebSocket closed" << std::endl;

        connected = false;
    });

    webSocket->onMessage([wws = std::weak_ptr(webSocket), this, username_](auto data) {
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

//        // If the id in the message returned from the server is the same as the user,
//        // it means that the server couldn't find the wanted user.
//        if (id == username_)
//        {
//            auto destIt = message.find("destination_id");
//
//            if (destIt == message.end())
//            {
//                std::cout << "Could not find user, destination ID is invalid" << std::endl;
//                return;
//            }
//            std::cout << "Could not find user: " << destIt->get<std::string>() << ". Operation cancelled." << std::endl;
//
//            if (auto badPc = this->state.GetPeerConnection(destIt->get<std::string>()))
//            {
//                badPc->close();
//            }
//            return;
//        }

        it = message.find("type");
        if (it == message.end())
            return;

        auto type = it->get<std::string>();

        if (auto jt = this->state.GetPeerConnection(id))
        {
        }
        else if (type == "offer")
        {
            std::cout << "Answering to " + id << std::endl;
            networkAPIEventBus.Publish(PeerRequestEvent(id));
        }
        else
        {
            std::cout << "Could not establish connection to " << id << std::endl;
            return;
        }

        if (type == "offer" || type == "answer")
        {
            auto sdp = message["description"].get<std::string>();
            networkAPIEventBus.Publish(RemoteDescriptionEvent(id, rtc::Description(sdp, type)));
        }
        else if (type == "candidate")
        {
            auto sdp = message["candidate"].get<std::string>();
            auto mid = message["mid"].get<std::string>();
            networkAPIEventBus.Publish(RemoteCandidateEvent(id, rtc::Candidate(sdp, mid)));
        }
    });

    const std::string wsPrefix = "wss://";
    const std::string url = wsPrefix + this->signalingServer + ":" +
                            this->signalingServerPort + "/" + username_;

    std::cout << "WebSocket URL is " << url << std::endl;
    webSocket->open(url);

    std::cout << "Waiting for signaling to be connected..." << std::endl;
    wsFuture.get();
}

void LibDatachannelConnectionAPI::OnConnected(std::function<void()> callback)
{
    this->onConnectedCb = callback;
}