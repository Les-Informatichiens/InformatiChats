//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "rtc/datachannel.hpp"
#include "rtc/peerconnection.hpp"

#include "LibDatachannelAPIEvents.h"
#include "Model/EventBus.h"
#include "Model/MessageDispatcher.h"
#include <Model/Models/PeerEventMessages.h>

#include "rtc/websocket.hpp"
#include "zpp_bits.h"

#include <utility>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>


static const std::string eventChannelName = "message";

class LibDatachannelPeer
{
public:
    LibDatachannelPeer(const std::string& peerId, const std::string& signalingId, EventBus& eventBus, const rtc::Configuration& config);
    ~LibDatachannelPeer();

    // delete copy constructor and assignment operator
    LibDatachannelPeer(const LibDatachannelPeer&) = delete;
    LibDatachannelPeer& operator=(const LibDatachannelPeer&) = delete;


    void Connect();
    void Disconnect();

    void SendMessage(const BaseMessage<MessageType>& message);

    template<typename... Ts, typename Handler>
    void SubscribeEvents(Handler&& handler)
    {
        this->messageDispatcher.RegisterHandlers<Ts...>(std::forward<Handler>(handler));
    }

    template<typename T, typename Handler>
    void SubscribeEvent(Handler&& handler)
    {
        this->messageDispatcher.RegisterHandler<T, Handler>(std::forward<Handler>(handler));
    }

    std::shared_ptr<rtc::DataChannel> CreateNegotiatedChannel(const std::string& label, int streamId);

    void SetRemoteDescription(rtc::Description description);
    void AddRemoteCandidate(rtc::Candidate candidate);

    void SetTextChannel(const std::shared_ptr<rtc::DataChannel>& dc);
    [[nodiscard]] std::shared_ptr<rtc::DataChannel> GetTextChannel() const;

    [[nodiscard]] const std::string& GetId() const;
    [[nodiscard]] const std::string& GetSignalingId() const;
    [[nodiscard]] std::optional<std::string> GetIpAddress() const;
    [[nodiscard]] bool IsConnected() const;
    [[nodiscard]] rtc::PeerConnection::State State();

    void OnConnected(std::function<void()> callback);
    void OnStateChange(std::function<void(rtc::PeerConnection::State)> callback);
    void OnMessage(std::function<void(BaseMessage<MessageType>&)> callback);
    void OnTextDcMessage(std::function<void(rtc::message_variant)> callback);

private:
    void SetDataChannel(std::shared_ptr<rtc::DataChannel> dc_);

private:
    std::string peerId;
    std::string signalingId;
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;

    std::shared_ptr<rtc::DataChannel> textDc = nullptr;

    EventBus& eventBus;
    MessageDispatcher<MessageType> messageDispatcher{};

    std::function<void()> onConnectedCb;
    std::function<void(rtc::PeerConnection::State)> onStateChangeCb;
    std::function<void(BaseMessage<MessageType>&)> onMessageCb;
    std::function<void(rtc::message_variant)> onTextDcMessageCb;
};
