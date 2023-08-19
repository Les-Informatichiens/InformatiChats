//
// Created by Jonathan Richard on 2023-07-27.
//

#pragma once

#include <View/console.h>
#include "ChatClient.h"

#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include <utility>


class Chat
{
public:
    explicit Chat(const ConnectionConfig& config);

    Chat() = default;

    void AttemptConnectionWithUsername(const char *newUsername);

    void SendMessageToPeer(const std::string &peerId, const char *message);

    void SetOnMessageRecieved(std::function<void(MessageReceivedEvent)> callback)
    {
        onMessageReceivedCallback = std::move(callback);
    };

    [[nodiscard]] const std::string &GetUsername() const { return username; };

    [[nodiscard]] bool IsConnected() const { return connected; };

    const std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> &
    GetPeerConnections() { return peerConnectionMap; };

private:
    void CreateDataChannel(std::shared_ptr<rtc::PeerConnection> &pc, const std::string &peerId);

    void RegisterDataChannel(const std::shared_ptr<rtc::DataChannel> &dc, const std::string &peerId);

    // Create and setup a PeerConnection
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string &peerId);

private:
    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    std::promise<void> wsPromise;
    std::shared_ptr<rtc::WebSocket> webSocket;

    std::string username;//

    bool connected{};

    std::function<void(MessageReceivedEvent)> onMessageReceivedCallback;
};
