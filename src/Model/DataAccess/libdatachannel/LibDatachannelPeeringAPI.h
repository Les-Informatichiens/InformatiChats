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

    void OnPeerConnectionStateChange(std::function<void (PeerConnectionStateChangeEvent)> callback) override;
    void OnPeerRequest(std::function<bool (std::string)> callback) override;

private:
    std::shared_ptr<Peer> CreatePeerConnection(const std::string&
                                                       connectionState);

public:
    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    rtc::Configuration rtcConfig;

    std::function<void (PeerConnectionStateChangeEvent)> onPeerConnectionStateChangeCb;
    std::function<void(std::string)> onPeerConnectedCb;
    std::function<bool(std::string)> onPeerRequestCb;
};