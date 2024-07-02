//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/DTO/ChannelDTO.h"
#include "Controller/DTO/RemoteUserDTO.h"
#include "Model/Models/PeerData.h"
#include <string>
#include <unordered_map>

struct ChannelViewModel
{
    std::unordered_map<std::string, RemoteUserDTO> peerDataMap;
    std::unordered_map<std::string, ChannelDTO> channelDataMap;
    std::unordered_map<std::string, ContactData> contactDataMap;
    std::string selectedPeerId;
    std::string userName;
};
