//
// Created by Jonathan Richard on 2023-09-16.
//

#pragma once

#include "rtc/datachannel.hpp"
#include "rtc/peerconnection.hpp"

#include "LibDatachannelAPIEvents.h"
#include "Model/EventBus.h"
#include "rtc/websocket.hpp"
#include "zpp_bits.h"
#include <unordered_map>
#include <utility>

struct Peer
{
    Peer(const std::string& peerId, EventBus& eventBus, const rtc::Configuration& config)
        : peerId(peerId), eventBus(eventBus)
    {
        this->pc = std::make_shared<rtc::PeerConnection>(config);

        this->pc->onDataChannel([this](rtc::shared_ptr<rtc::DataChannel> dataChannel) {
            std::cout << "Data channel received" << std::endl;
            if (dataChannel->label() == "ping")
                this->SetDataChannel(dataChannel);
        });

        this->pc->onStateChange([this](rtc::PeerConnection::State state) {
            std::cout << "Conn state = " << state << std::endl;
            this->onStateChangeCb(state);
        });
        this->pc->onLocalDescription([this, peerId](const rtc::Description& description) {
            std::cout << "SENDING LOCAL DESC" << std::endl;
            this->eventBus.Publish(SendLocalDescriptionEvent(peerId, description.typeString(), description));
        });
        this->pc->onLocalCandidate([this, peerId](const rtc::Candidate& candidate) {
            this->eventBus.Publish(SendLocalCandidateEvent(peerId, std::string(candidate), candidate.mid()));
        });
    }

    ~Peer()
    {
        this->Disconnect();
    }

    void Connect()
    {
        this->SetDataChannel(this->pc->createDataChannel("ping"));
    }

    void Disconnect()
    {
        if (dc)
        {
            this->dc->close();
            this->dc.reset();
        }
    }

    [[nodiscard]] bool IsConnected() const
    {
        return this->dc && this->dc->isOpen();
    }

    void OnConnected(std::function<void()> callback)
    {
        this->onConnectedCb = std::move(callback);
    }

    void OnStateChange(std::function<void(rtc::PeerConnection::State)> callback)
    {
        this->onStateChangeCb = std::move(callback);
    }

    void SetDataChannel(std::shared_ptr<rtc::DataChannel> dc_)
    {
        this->dc = std::move(dc_);

        auto openCallback = [this]() {
            std::cout << "Data channel open" << std::endl;
            if (this->onConnectedCb)
                this->onConnectedCb();
        };

        auto closeCallback = [this]() {
            std::cout << "Data channel closed" << std::endl;
        };

        if (this->dc->isOpen())
            openCallback();
        else
            this->dc->onOpen(openCallback);

        this->dc->onClosed(closeCallback);

        this->dc->onMessage([this, wdc = std::weak_ptr(this->dc)](auto m) {
            //        std::cout << std::get<std::string>(m) << std::endl;
            zpp::bits::in in(std::get<rtc::binary>(m));
            int opcode;
            in(opcode).or_throw();
            if (opcode == 0)
            {
                std::cout << "MAX DATACHANNEL STREAM = " << this->pc->maxDataChannelId() << std::endl;
                rtc::DataChannelInit init;
                init.negotiated = true;
                init.id = 42;
                auto textDc = this->pc->createDataChannel("text", init);
                this->eventBus.Publish(OnTextChannelEvent(this->peerId, textDc));
                std::cout << "Received text request to " + this->peerId + ", sending response." << std::endl;
                auto [data, out] = zpp::bits::data_out();
                out(1).or_throw();
                this->dc->send(data);
            }
            else if (opcode == 1)
            {
                std::cout << "MAX DATACHANNEL STREAM = " << this->pc->maxDataChannelId() << std::endl;
                rtc::DataChannelInit init;
                init.negotiated = true;
                init.id = 42;
                auto textDc = this->pc->createDataChannel("test", init);
                std::cout << "Received text approval from " + this->peerId << std::endl;
                this->eventBus.Publish(OnTextChannelEvent(this->peerId, textDc));
            }
        });
    }

    std::string peerId;
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
    EventBus& eventBus;

    std::function<void()> onConnectedCb;
    std::function<void(rtc::PeerConnection::State)> onStateChangeCb;
};

class LibDatachannelState
{
public:
    void Reset();

    virtual ~LibDatachannelState();
    void SetPeerChannel(const std::string& peerId, const std::shared_ptr<rtc::DataChannel>& dc);
    void RegisterPeerConnection(const std::string& peerId, const std::shared_ptr<rtc::PeerConnection>& pc);
    void ClosePeerConnection(const std::string& peerId);
    std::shared_ptr<rtc::PeerConnection> GetPeerConnection(const std::string& peerId);
    void RegisterPeer(const std::shared_ptr<Peer>& peer);
    std::shared_ptr<Peer> GetPeer(const std::string& peerId);

public:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<Peer>> peerMap;
};
