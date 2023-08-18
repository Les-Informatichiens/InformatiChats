//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "../../View/Views/IView.h"
#include "rtc/rtc.hpp"
class ChannelViewModel {

public:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::string userName;
};
