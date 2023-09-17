//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include <string>

class IConnectionAPI
{
public:
    virtual void ConnectWithUsername(const std::string& username) = 0;
    virtual void AttemptToConnectToPeer(const std::string& peerId) = 0;
};
