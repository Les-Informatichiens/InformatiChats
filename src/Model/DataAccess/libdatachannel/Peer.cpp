//
// Created by Jonathan Richard on 2023-09-22.
//

#include "Peer.h"

#include <utility>

Peer::Peer(const std::string& peerId, EventBus& eventBus, const rtc::Configuration& config)
    : peerId(peerId), eventBus(eventBus)
{
    this->pc = std::make_shared<rtc::PeerConnection>(config);

    this->pc->onDataChannel([this](rtc::shared_ptr<rtc::DataChannel> dataChannel) {
        std::cout << "Data channel received" << std::endl;
        if (dataChannel->label() == "ping")
            this->SetDataChannel(dataChannel);
    });

    this->pc->onStateChange([this](rtc::PeerConnection::State state) {
        this->onStateChangeCb(state);
    });
    this->pc->onLocalDescription([this, peerId](const rtc::Description& description) {
        this->eventBus.Publish(SendLocalDescriptionEvent(peerId, description.typeString(), description));
    });
    this->pc->onLocalCandidate([this, peerId](const rtc::Candidate& candidate) {
        this->eventBus.Publish(SendLocalCandidateEvent(peerId, std::string(candidate), candidate.mid()));
    });
}

Peer::~Peer()
{
    this->Disconnect();
}

void Peer::Connect()
{
    this->SetDataChannel(this->pc->createDataChannel("ping"));
}

void Peer::Disconnect()
{
    if (dc)
    {
        this->dc->close();
        this->dc.reset();
    }
}

void Peer::Send(const BaseMessage<MessageType>& message)
{
    if (!this->IsConnected())
        return;

    auto [data, out] = zpp::bits::data_out();
    out(message.GetOpcode(), message.Serialize()).or_throw();
    this->dc->send(data);
}

void Peer::SetDataChannel(std::shared_ptr<rtc::DataChannel> dc_)
{
    this->dc = std::move(dc_);

    auto openCallback = [this]() {
        std::cout << "Data channel open" << std::endl;
        if (this->onConnectedCb)
            this->onConnectedCb();
    };

    auto closeCallback = [this]() {
        std::cout << "Data channel closed" << std::endl;
    };

    if (this->dc->isOpen())
        openCallback();
    else
        this->dc->onOpen(openCallback);

    this->dc->onClosed(closeCallback);

    this->dc->onMessage([this, wdc = std::weak_ptr(this->dc)](auto data) {
        //        std::cout << std::get<std::string>(m) << std::endl;
        MessageType opcode;
        std::vector<std::byte> payload;
        zpp::bits::in in(std::get<std::vector<std::byte>>(data));
        in(opcode, payload).or_throw();

        auto dispatchPayload = [this, &payload]<typename T>() {
            T m;
            m.Deserialize(payload);
            this->messageDispatcher.Dispatch(m);
        };

        switch (opcode)
        {
            case TextRequest::opcode:
                dispatchPayload.template operator()<TextRequest>();
                break;
            case TextResponse::opcode:
                dispatchPayload.template operator()<TextResponse>();
                break;
        }
    });
}

void Peer::SetRemoteDescription(rtc::Description description)
{
    this->pc->setRemoteDescription(std::move(description));
}

void Peer::AddRemoteCandidate(rtc::Candidate candidate)
{
    this->pc->addRemoteCandidate(std::move(candidate));
}

const std::string& Peer::GetId() const
{
    return this->peerId;
}

bool Peer::IsConnected() const
{
    return this->dc && this->dc->isOpen();
}

rtc::PeerConnection::State Peer::State()
{
    return this->pc->state();
}

void Peer::OnConnected(std::function<void()> callback)
{
    if (this->IsConnected())
        callback();
    else
        this->onConnectedCb = std::move(callback);
}

void Peer::OnStateChange(std::function<void(rtc::PeerConnection::State)> callback)
{
    this->onStateChangeCb = std::move(callback);
}
std::shared_ptr<rtc::DataChannel> Peer::CreateNegotiatedChannel(const std::string& label, int streamId)
{
    if (!this->IsConnected())
    {
        return nullptr;
    }

    rtc::DataChannelInit init;
    init.negotiated = true;
    init.id = streamId;
    return this->pc->createDataChannel(label, init);
}
