//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IConnexionController.h"
#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include <Controller/ViewModels/ConnexionViewModel.h>

class ConnexionController : public IConnexionController
{
public:
    ConnexionViewModel getViewModel() override;

    bool IsConnected() override { return connected; };

    void AttemptConnectionWithUsername(const std::string &newUsername) override;

private:
    struct MessageReceivedEvent
    {
        std::string senderId;
        std::string content;
    };

    std::string signalingServer;
    std::string signalingServerPort;
    rtc::Configuration rtcConfig;
    std::function<void(MessageReceivedEvent)> onMessageReceivedCallback;
    std::shared_ptr<rtc::WebSocket> webSocket;
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::string username;
    std::promise<void> wsPromise;
    bool connected{false};
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string &peerId);
    static constexpr int maxNameLength{32};
    void RegisterDataChannel(const std::shared_ptr<rtc::DataChannel> &dc, const std::string &peerId);
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;
};
