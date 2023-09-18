//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "IChatAPI.h"
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

    void Init(const ConnectionConfig& config_) override;
    
    void ConnectWithUsername(const std::string& username_) override;
    void Disconnect() override;

    bool IsConnected() override;

    void OnConnected(std::function<void ()> callback) override;

private:
    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    std::string signalingServer;
    std::string signalingServerPort;

    std::shared_ptr<rtc::WebSocket> webSocket;
    std::promise<void> wsPromise;
    bool connected{};

    std::function<void(void)> onConnectedCb;
};
