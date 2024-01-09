//
// Created by Jonathan Richard on 2023-09-17.
//

#pragma once


#include "LibDatachannelState.h"
#include "Model/DataAccess/IPeeringAPI.h"
#include "Model/EventBus.h"

#include "rtc/peerconnection.hpp"

class LibDatachannelPeeringAPI : public IPeeringAPI
{
public:
    explicit LibDatachannelPeeringAPI(LibDatachannelState& state, EventBus& networkAPIEventBus);

    void Init(const PeeringConfig& peeringConfig) override;

    void OpenPeerConnection(const std::string& peerId, std::function<void()> onReady) override;
    void ClosePeerConnection(const std::string& peerId) override;

    void SendMessage(const std::string& peerId, const BaseMessage<MessageType>& message) override;

    [[nodiscard]] std::optional<std::string> GetPeerIpAddress(const std::string& peerId) override;

    void OnPeerConnectionStateChange(std::function<void (PeerConnectionStateChangeEvent)> callback) override;
    void OnPeerRequest(std::function<bool(const std::string&)> callback) override;
    void OnNewPeer(std::function<void(const std::string&)> callback) override;
    void OnPeerMessage(const std::string& peerId, std::function<void(BaseMessage<MessageType>&)> callback) override;
    void OnPeerConnected(const std::string& peerId, std::function<void()> callback) override;

private:
    std::shared_ptr<LibDatachannelPeer> CreatePeerConnection(const std::string&
                                                       connectionState);

public:
    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    rtc::Configuration rtcConfig;

    std::function<void (PeerConnectionStateChangeEvent)> onPeerConnectionStateChangeCb;
    std::function<bool(const std::string&)> onPeerRequestCb;
    std::function<void(const std::string&)> onNewPeerCb;
};