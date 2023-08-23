//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IChannelController.h"

#include "Model/Model.h"
#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include <cstdio>
#include <utility>


class ChannelController : public IChannelController
{
public:
    explicit ChannelController(Model& model);

    ChannelViewModel GetViewModel() override;

    void AddNewChatPeer(const std::string& peerName) override;
    void SetSelectedPeerId(const std::string& peerId) override;

private:
    Model& model;
};
