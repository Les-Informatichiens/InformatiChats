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

    void AttemptToConnectToPeer(std::string &peerName) override;

    std::string GetUsername() override;

private:
    struct MessageReceivedEvent
    {
        std::string senderId;
        std::string content;
    };

    std::string username;
    rtc::Configuration rtcConfig;
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::shared_ptr<rtc::WebSocket> webSocket;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    std::function<void(MessageReceivedEvent)> onMessageReceivedCallback;

    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string &peerId);

    void RegisterDataChannel(const std::shared_ptr<rtc::DataChannel> &dc, const std::string &peerId);

    void CreateDataChannel(std::shared_ptr<rtc::PeerConnection> &pc, const std::string &peerId);

    Model& model;
};
