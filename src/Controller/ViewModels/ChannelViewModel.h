//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "rtc/rtc.hpp"
#include <View/Views/IView.h>

struct ChannelViewModel
{
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::string userName;
};
