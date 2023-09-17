//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include <string>
#include <functional>


class IConnectionAPI
{
public:
    virtual void ConnectWithUsername(const std::string& username) = 0;
    virtual void OnConnected(std::function<void(void)> callback) = 0;
    virtual void OnPeerRequest(std::function<bool(std::string)> callback) = 0;
};
