//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelState.h"
#include "nlohmann/json.hpp"

#include <utility>

void LibDatachannelState::Reset()
{
    this->peerConnectionMap.clear();
}

std::shared_ptr<rtc::PeerConnection> LibDatachannelState::CreatePeerConnection(const std::string& peerId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(this->rtcConfig);
    pc->onLocalDescription([wss = std::weak_ptr(this->webSocket), peerId](const rtc::Description& description) {
        nlohmann::json message = {{"id", peerId},
                                  {"type", description.typeString()},
                                  {"description", std::string(description)}};
        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onLocalCandidate([wss = std::weak_ptr(this->webSocket), peerId](const rtc::Candidate& candidate) {
        nlohmann::json message = {{"id", peerId},
                                  {"type", "candidate"},
                                  {"candidate", std::string(candidate)},
                                  {"mid", candidate.mid()}};

        if (auto ws = wss.lock())
            ws->send(message.dump());
    });

    pc->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        this->onPcState(peerId, state);
    });

    pc->onDataChannel([peerId, this](const std::shared_ptr<rtc::DataChannel>& dc) {
        std::cout << "DataChannel from " << peerId << " received with label \"" << dc->label() << "\""
                  << std::endl;

        this->ProcessDataChannel(peerId, dc);
    });
    this->peerConnectionMap.emplace(peerId, pc);
    return pc;
}

void LibDatachannelState::ClosePeerConnection(const std::string& peerId)
{
    auto pcIt = this->peerConnectionMap.find(peerId);
    if (pcIt == this->peerConnectionMap.end())
    {
        return;
    }
    pcIt->second.reset();
    peerConnectionMap.erase(pcIt);
}

std::shared_ptr<rtc::PeerConnection> LibDatachannelState::GetPeerConnection(const std::string& peerId)
{
    auto pcIt = this->peerConnectionMap.find(peerId);
    if (pcIt == this->peerConnectionMap.end())
    {
        return nullptr;
    }
    return pcIt->second;
}

void LibDatachannelState::SetRTCConfig(rtc::Configuration config)
{
    this->rtcConfig = std::move(config);
}

void LibDatachannelState::ProcessDataChannel(const std::string& peerId, std::shared_ptr<rtc::DataChannel> dataChannel)
{
    if (dataChannel->label() == "text")
    {
        this->onTextDc(peerId, dataChannel);
    }
    else if (dataChannel->label() == "event")
    {
        this->onEventDc(peerId, dataChannel);
    }
}

void LibDatachannelState::OnPeerConnectionState(std::function<void(std::string, rtc::PeerConnection::State)> callback)
{
    this->onPcState = std::move(callback);
}

void LibDatachannelState::OnTextDataChannel(std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> callback)
{
    this->onTextDc = std::move(callback);
}

void LibDatachannelState::OnEventChannel(std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> callback)
{
    this->onEventDc = std::move(callback);
}

std::shared_ptr<rtc::WebSocket> LibDatachannelState::GetSignalingSocket() {
    return this->webSocket;
}
