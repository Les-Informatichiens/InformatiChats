//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include <string>
#include <functional>

struct ConnectionConfig
{
    std::string signalingServer;
    std::string signalingServerPort;
};

class IConnectionAPI
{
public:
    virtual void Init(const ConnectionConfig& config_) = 0;

    virtual void ConnectWithUsername(const std::string& username) = 0;
    virtual void Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void OnConnected(std::function<void(void)> callback) = 0;
};
