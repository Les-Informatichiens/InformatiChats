//
// Created by Jonathan Richard on 2023-09-22.
//

#include "LibDatachannelPeer.h"


LibDatachannelPeer::LibDatachannelPeer(const std::string& peerId, const std::string& signalingId, EventBus& eventBus, const rtc::Configuration& config)
    : peerId(peerId), signalingId(signalingId), eventBus(eventBus)
{
    this->pc = std::make_shared<rtc::PeerConnection>(config);

    this->pc->onDataChannel([this](rtc::shared_ptr<rtc::DataChannel> dataChannel) {
        std::cout << "Data channel received" << std::endl;
        if (dataChannel->label() == eventChannelName)
            this->SetDataChannel(dataChannel);
    });

    this->pc->onStateChange([this](rtc::PeerConnection::State state) {
        this->onStateChangeCb(state);
    });
    this->pc->onLocalDescription([this, signalingId](const rtc::Description& description) {
        this->eventBus.Publish(SendLocalDescriptionEvent(signalingId, description.typeString(), description));
    });
    this->pc->onLocalCandidate([this, signalingId](const rtc::Candidate& candidate) {
        this->eventBus.Publish(SendLocalCandidateEvent(signalingId, std::string(candidate), candidate.mid()));
    });
}

LibDatachannelPeer::~LibDatachannelPeer()
{
    std::cout << "LibDatachannelPeer destroyed (" <<  this->peerId << ")" << std::endl;
    this->Disconnect();
}

void LibDatachannelPeer::Connect()
{
    this->SetDataChannel(this->pc->createDataChannel(eventChannelName));
}

void LibDatachannelPeer::Disconnect()
{
    if (dc)
    {
        this->dc->close();
        this->dc.reset();
    }
}

void LibDatachannelPeer::SendMessage(const BaseMessage<MessageType>& message)
{
    if (!this->IsConnected())
        return;

    auto [data, out] = zpp::bits::data_out();
    out(message.GetOpcode(), message.Serialize()).or_throw();
    this->dc->send(data);
}

std::shared_ptr<rtc::DataChannel> LibDatachannelPeer::CreateNegotiatedChannel(const std::string& label, int streamId)
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

void LibDatachannelPeer::SetDataChannel(std::shared_ptr<rtc::DataChannel> dc_)
{
    this->dc = std::move(dc_);

    auto openCallback = [this]() {
        std::cout << "Events channel open" << std::endl;
        if (this->onConnectedCb)
            this->onConnectedCb();
    };

    auto closeCallback = []() {
        std::cout << "Events channel closed" << std::endl;
    };

    if (this->dc->isOpen())
        openCallback();
    else
        this->dc->onOpen(openCallback);

    this->dc->onClosed(closeCallback);

    this->dc->onMessage([this, wdc = std::weak_ptr(this->dc)](auto data) {
        MessageType opcode;
        std::vector<std::byte> payload;
        zpp::bits::in in(std::get<std::vector<std::byte>>(data));
        in(opcode, payload).or_throw();

        auto dispatchPayload = [this, &payload]<typename T>() {
            T m;
            m.Deserialize(payload);
            this->messageDispatcher.Dispatch(m);
            this->onMessageCb(m);
        };

        switch (opcode)
        {
            case ResetExchange::opcode:
                dispatchPayload.template operator()<ResetExchange>();
                break;
            case TextRequest::opcode:
                dispatchPayload.template operator()<TextRequest>();
                break;
            case TextRequestResponse::opcode:
                dispatchPayload.template operator()<TextRequestResponse>();
                break;
            case TextResponseAck::opcode:
                dispatchPayload.template operator()<TextResponseAck>();
                break;
            case AuthChallengeRequest::opcode:
                dispatchPayload.template operator()<AuthChallengeRequest>();
                break;
            case AuthChallengeAnswer::opcode:
                dispatchPayload.template operator()<AuthChallengeAnswer>();
                break;
            case ContactRequest::opcode:
                dispatchPayload.template operator()<ContactRequest>();
                break;
            case ContactRequestResponse::opcode:
                dispatchPayload.template operator()<ContactRequestResponse>();
                break;
            case UserProfileRequest::opcode:
                dispatchPayload.template operator()<UserProfileRequest>();
                break;
            case UserProfileRequestResponse::opcode:
                dispatchPayload.template operator()<UserProfileRequestResponse>();
                break;
            case PublicKeyRequest::opcode:
                dispatchPayload.template operator()<PublicKeyRequest>();
                break;
            case PublicKeyRequestResponse::opcode:
                dispatchPayload.template operator()<PublicKeyRequestResponse>();
                break;
            case ChannelRequest::opcode:
                dispatchPayload.template operator()<ChannelRequest>();
                break;
            case ChannelRequestResponse::opcode:
                dispatchPayload.template operator()<ChannelRequestResponse>();
                break;
            case ChannelUserJoin::opcode:
                dispatchPayload.template operator()<ChannelUserJoin>();
                break;
            case ChannelUserLeave::opcode:
                dispatchPayload.template operator()<ChannelUserLeave>();
                break;
            case SignedMessage::opcode:
                dispatchPayload.template operator()<SignedMessage>();
                break;
        }
    });
}

void LibDatachannelPeer::SetRemoteDescription(rtc::Description description)
{
    this->pc->setRemoteDescription(std::move(description));
}

void LibDatachannelPeer::AddRemoteCandidate(rtc::Candidate candidate)
{
    this->pc->addRemoteCandidate(std::move(candidate));
}

void LibDatachannelPeer::SetTextChannel(const std::shared_ptr<rtc::DataChannel>& dc)
{
    this->textDc = dc;
    this->textDc->onMessage([this](const rtc::message_variant& data) {
        if (!this->onTextDcMessageCb)
            return;

        this->onTextDcMessageCb(data);
    });
}

std::shared_ptr<rtc::DataChannel> LibDatachannelPeer::GetTextChannel() const
{
    return this->textDc;
}

const std::string& LibDatachannelPeer::GetId() const
{
    return this->peerId;
}

const std::string& LibDatachannelPeer::GetSignalingId() const
{
    return this->signalingId;
}

std::optional<std::string> LibDatachannelPeer::GetIpAddress() const
{
    return this->pc->remoteAddress();
}

bool LibDatachannelPeer::IsConnected() const
{
    return this->dc && this->dc->isOpen();
}

rtc::PeerConnection::State LibDatachannelPeer::State()
{
    return this->pc->state();
}

void LibDatachannelPeer::OnConnected(std::function<void()> callback)
{
    if (this->IsConnected())
        callback();
    else
        this->onConnectedCb = std::move(callback);
}

void LibDatachannelPeer::OnStateChange(std::function<void(rtc::PeerConnection::State)> callback)
{
    this->onStateChangeCb = std::move(callback);
}

void LibDatachannelPeer::OnMessage(std::function<void(BaseMessage<MessageType>&)> callback)
{
    this->onMessageCb = std::move(callback);
}

void LibDatachannelPeer::OnTextDcMessage(std::function<void(rtc::message_variant)> callback)
{
    this->onTextDcMessageCb = std::move(callback);
}
