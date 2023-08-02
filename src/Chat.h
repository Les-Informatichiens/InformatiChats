//
// Created by Jonathan Richard on 2023-07-27.
//

#ifndef INFORMATICHATS_CHAT_H
#define INFORMATICHATS_CHAT_H

#include "console.h"
#include "nlohmann/json_fwd.hpp"
#include "nlohmann/json.hpp"
#include <rtc/rtc.hpp>

struct ConnectionConfig
{
    std::string stunServer;
    std::string stunServerPort;
    std::string signalingServer;
    std::string signalingServerPort;
};

class Chat {
public:

    explicit Chat(const ConnectionConfig& config);

    void AttemptConnectionWithUsername(const char* newUsername);

    void AttemptToConnectToPeer(const std::string& peerId);
    void SendMessageToPeer(const std::string& peerId, const char* message);

    const std::string& GetUsername() const { return username; };
    bool IsConnected() const { return connected; };

private:

    // Create and setup a PeerConnection
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string& userId);

private:
    rtc::Configuration rtcConfig;

    std::string signalingServer;
    std::string signalingServerPort;

    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    std::promise<void> wsPromise;
    std::shared_ptr<rtc::WebSocket> webSocket;

    std::string username;

    bool connected;
};


#endif //INFORMATICHATS_CHAT_H
