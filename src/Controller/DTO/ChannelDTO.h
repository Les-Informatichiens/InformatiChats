//
// Created by Jonathan Richard on 2024-01-03.
//

#pragma once


#include "RemoteUserDTO.h"


#include <optional>
#include <string>
#include <vector>

struct ChannelMemberDTO
{
    std::string fingerprint;
    std::string role;
    std::optional<RemoteUserDTO> peer;
};

struct ChannelDTO
{
    std::string uuid;
    std::string name;
    std::string localName;
    std::string description;
    std::vector<ChannelMemberDTO> members;
};