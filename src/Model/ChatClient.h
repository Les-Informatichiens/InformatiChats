#pragma once

#include "IChatClient.h"

#include "PeerData.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <rtc/rtc.hpp>
#include <string>


class ChatClient : public IChatClient
{
public:
    void Reset() override;

    void Init(const ConnectionConfig& config) override;

    bool ICEServerExists() const override;

    bool IsConnected() const override { return this->connected; };

    void AttemptConnectionWithUsername(const std::string& newUsername) override;

    void AttemptToConnectToPeer(const std::string& peerId) override;

    void SetOnPeerConnectionStateChange(std::function<void(PeerConnectionStateChangeEvent)> callback) override;
    void SetOnMessageReceived(std::function<void(MessageReceivedEvent)> callback) override;

private:
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string& peerId);
    void RegisterDataChannel(const std::shared_ptr<rtc::DataChannel>& dc, const std::string& peerId);
    void CreateDataChannel(std::shared_ptr<rtc::PeerConnection>& pc, const std::string& peerId);

private:
    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::function<void(PeerConnectionStateChangeEvent)> onPeerConnectionStateChangeCallback;
    std::function<void(MessageReceivedEvent)> onMessageReceivedCallback;

    std::string username;
    std::shared_ptr<rtc::WebSocket> webSocket;
    std::promise<void> wsPromise;

    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    bool connected{false};
};
