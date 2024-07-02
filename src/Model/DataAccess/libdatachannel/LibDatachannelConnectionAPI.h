//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "LibDatachannelState.h"
#include "Model/DataAccess/IConnectionAPI.h"
#include "Model/EventBus.h"
#include "Model/Models/Exchange.h"

#include "rtc/peerconnection.hpp"
#include "rtc/websocket.hpp"

#include "nlohmann/json.hpp"
#include <future>
#include <iostream>
#include <memory>

class LibDatachannelConnectionAPI : public IConnectionAPI
{
public:
    explicit LibDatachannelConnectionAPI(LibDatachannelState& state, EventBus& networkAPIEventBus);

    void Init(const ConnectionConfig& config_) override;
    
    void ConnectWithUsername(const std::string& username_, const std::string& publicKey) override;
    void Disconnect() override;

    bool IsConnected() override;

    void OnConnected(std::function<void()> callback) override;
    void QueryClientsByPublicKey(std::string publicKey, std::function<void(nlohmann::json)> callback) override;
    void QueryDiscoverableClients(std::function<void(nlohmann::json)> callback) override;

    LibDatachannelState& state;
    EventBus& networkAPIEventBus;

    std::string signalingServer;
    std::string signalingServerPort;

    std::shared_ptr<rtc::WebSocket> webSocket;
    std::promise<void> wsPromise;
    bool connected{};
    std::string clientId;

    ExchangeManager exchangeManager{};

    std::function<void(void)> onConnectedCb;
};
