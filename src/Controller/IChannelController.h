//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "ViewModels/ChannelViewModel.h"
#include <string>

class IChannelController {
public:
    virtual ChannelViewModel getViewModel() = 0;
    virtual void AttemptToConnectToPeer(std::string& peerId) = 0;
};