//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "IConnectionAPI.h"
#include "LibDatachannelState.h"
#include "Model/EventBus.h"

#include <rtc/peerconnection.hpp>
#include <rtc/websocket.hpp>

#include <future>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

class LibDatachannelConnectionAPI : public IConnectionAPI
{
public:
    explicit LibDatachannelConnectionAPI(LibDatachannelState& state, EventBus& networkAPIEventBus);

    void ConnectWithUsername(const std::string& username_) override;
    void OnConnected(std::function<void ()> callback) override;

private:
    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::promise<void> wsPromise;
    bool connected{};

    std::function<void(void)> onConnectedCb;
};
