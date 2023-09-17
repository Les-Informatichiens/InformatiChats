//
// Created by Jonathan Richard on 2023-09-16.
//

#include "LibDatachannelState.h"

#include <utility>

std::shared_ptr<rtc::PeerConnection> LibDatachannelState::CreatePeerConnection(const std::string& peerId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(this->rtcConfig);

    pc->onStateChange([this, peerId, pc](rtc::PeerConnection::State state) {
        std::cout << "State: " << state << std::endl;
        //        this->onPeerConnectionStateChangeCallback(PeerConnectionStateChangeEvent{peerId, static_cast<ConnectionState>(state)});
        if (state == rtc::PeerConnection::State::Closed ||
            state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed)
        {
            if (this->peerConnectionMap.find(peerId) != this->peerConnectionMap.end())
            {
                this->peerConnectionMap.erase(peerId);
                // should maybe close the open datachannels too? needs testing
            }
        }
    });

    pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state) {
        std::cout << "Gathering State: " << state << std::endl;
    });

    pc->onDataChannel([peerId, this](const std::shared_ptr<rtc::DataChannel>& dc) {
        std::cout << "DataChannel from " << peerId << " received with label \"" << dc->label() << "\""
                  << std::endl;

        OnDataChannel(peerId, dc);
    });

    this->peerConnectionMap.emplace(peerId, pc);
    return pc;
}

void LibDatachannelState::ClosePeerConnection(const std::string& peerId)
{
}

std::shared_ptr<rtc::PeerConnection> LibDatachannelState::GetPeerConnection(const std::string& peerId)
{
    auto pcIt = peerConnectionMap.find(peerId);
    if (pcIt == peerConnectionMap.end())
    {
        return nullptr;
    }
    return pcIt->second;
}

void LibDatachannelState::SetRTCConfig(rtc::Configuration config)
{
    this->rtcConfig = std::move(config);
}

void LibDatachannelState::OnDataChannel(const std::string& peerId, std::shared_ptr<rtc::DataChannel> dataChannel)
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

void LibDatachannelState::OnTextDataChannel(std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> callback)
{
    this->onTextDc = std::move(callback);
}

void LibDatachannelState::OnEventChannel(std::function<void(std::string, std::shared_ptr<rtc::DataChannel>)> callback)
{
    this->onEventDc = std::move(callback);
}
