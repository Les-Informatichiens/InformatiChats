//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "rtc/datachannel.hpp"
#include "rtc/peerconnection.hpp"

#include "LibDatachannelAPIEvents.h"
#include "Model/EventBus.h"
#include "Model/MessageDispatcher.h"
#include "rtc/websocket.hpp"
#include "zpp_bits.h"
#include <unordered_map>
#include <utility>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

enum class MessageType
{
    TextRequest,
    TextResponse
};

class TextRequest final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::TextRequest;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};

class TextResponse final : public BaseMessage<MessageType>
{
public:
    static const constexpr MessageType opcode = MessageType::TextResponse;

public:
    MessageType GetOpcode() const override { return opcode; };
    std::vector<std::byte> Serialize() const override { return {}; };
    void Deserialize(const std::vector<std::byte>& data) const override{};
};


class Peer
{
public:
    Peer(const std::string& peerId, EventBus& eventBus, const rtc::Configuration& config);
    ~Peer();

    void Connect();
    void Disconnect();

    void SendMessage(const BaseMessage<MessageType>& message);

    template<typename T, typename Handler>
    void SubscribeEvent(Handler&& handler)
    {
        this->messageDispatcher.RegisterHandler<T, Handler>(std::forward<Handler>(handler));
    }

    std::shared_ptr<rtc::DataChannel> CreateNegotiatedChannel(const std::string& label, int streamId);

    void SetRemoteDescription(rtc::Description description);
    void AddRemoteCandidate(rtc::Candidate candidate);

    [[nodiscard]] const std::string& GetId() const;
    [[nodiscard]] bool IsConnected() const;
    [[nodiscard]] rtc::PeerConnection::State State();

    void OnConnected(std::function<void()> callback);
    void OnStateChange(std::function<void(rtc::PeerConnection::State)> callback);

private:
    void SetDataChannel(std::shared_ptr<rtc::DataChannel> dc_);

private:
    std::string peerId;
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
    EventBus& eventBus;
    MessageDispatcher<MessageType> messageDispatcher{};

    std::function<void()> onConnectedCb;
    std::function<void(rtc::PeerConnection::State)> onStateChangeCb;
};
