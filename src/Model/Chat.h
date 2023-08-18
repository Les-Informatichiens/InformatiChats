//
// Created by Jonathan Richard on 2023-07-27.
//

#ifndef INFORMATICHATS_CHAT_H
#define INFORMATICHATS_CHAT_H

#include <View/console.h>

#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include <utility>

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

class Chat
{
public:

    explicit Chat(const ConnectionConfig& config);
    explicit Chat() = default;

    void AttemptConnectionWithUsername(const char* newUsername);

    void AttemptToConnectToPeer(const std::string& peerId);
    void SendMessageToPeer(const std::string& peerId, const char* message);

    void SetOnMessageRecieved(std::function<void(MessageReceivedEvent)> callback) { onMessageReceivedCallback = std::move(callback); };

    [[nodiscard]] const std::string& GetUsername() const { return username; };
    [[nodiscard]] bool IsConnected() const { return connected; };

    const std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>>& GetPeerConnections() { return peerConnectionMap; };

private:

    void CreateDataChannel(std::shared_ptr<rtc::PeerConnection>& pc, const std::string& peerId);
    void RegisterDataChannel(const std::shared_ptr<rtc::DataChannel>& dc, const std::string& peerId);

    // Create and setup a PeerConnection
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string& peerId);

private:
    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    std::promise<void> wsPromise;
    std::shared_ptr<rtc::WebSocket> webSocket;

    std::string username;

    bool connected{};

    std::function<void(MessageReceivedEvent)> onMessageReceivedCallback;
};


#endif //INFORMATICHATS_CHAT_H
