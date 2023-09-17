//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include <rtc/datachannel.hpp>
#include <rtc/peerconnection.hpp>

#include <unordered_map>
#include <utility>
#include "rtc/websocket.hpp"

class LibDatachannelState
{
public:

    void Reset();

    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(const std::string& peerId);
    void ClosePeerConnection(const std::string& peerId);
    std::shared_ptr<rtc::PeerConnection> GetPeerConnection(const std::string& peerId);

    std::shared_ptr<rtc::WebSocket> GetSignalingSocket();
    void SetRTCConfig(rtc::Configuration config);

    void OnPeerConnectionState(std::function<void(std::string, rtc::PeerConnection::State)> callback);
    void OnTextDataChannel(std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> callback);
    void OnEventChannel(std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> callback);

private:
    void ProcessDataChannel(const std::string& peerId, std::shared_ptr<rtc::DataChannel> dataChannel);

private:
    rtc::Configuration rtcConfig;

    std::shared_ptr<rtc::WebSocket> webSocket;

    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;

    std::function<void(std::string, rtc::PeerConnection::State)> onPcState;

    std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> onTextDc;
    std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> onEventDc;
};
