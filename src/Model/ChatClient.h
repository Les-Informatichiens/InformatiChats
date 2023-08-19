#pragma once

#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>
#include <memory>
#include <string>


struct MessageReceivedEvent
{
    std::string senderId;
    std::string content;
};

struct ConnectionConfig
{
    std::string stunServer;
    std::string stunServerPort;
    std::string signalingServer;
    std::string signalingServerPort;
};


class ChatClient
{
public:
    void Init(const ConnectionConfig& config);

    bool ICEServerExists() const;

    bool IsConnected() const { return this->connected; };

    void AttemptConnectionWithUsername(const std::string &newUsername);

private:
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string &peerId);
    void RegisterDataChannel(const std::shared_ptr<rtc::DataChannel> &dc, const std::string &peerId);

private:
    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::function<void(MessageReceivedEvent)> onMessageReceivedCallback;

    std::string username;
    std::shared_ptr<rtc::WebSocket> webSocket;
    std::promise<void> wsPromise;

    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    bool connected{false};
};


