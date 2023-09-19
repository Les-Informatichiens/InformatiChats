//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Model/Models/PeerData.h"
#include <string>
#include <unordered_map>

struct ChannelViewModel
{
    std::unordered_map<std::string, PeerData> peerDataMap;
    std::string selectedPeerId;
    std::string userName;
};
