//
// Created by Jonathan Richard on 2023-09-17.
//

#pragma once

#include "LibDatachannelPeer.h"
#include "Model/EventBus.h"
#include "rtc/rtc.hpp"
#include <utility>

struct PeerRequestEvent : public EventData
{
    std::string peerId;
    std::string fingerprint;

    explicit PeerRequestEvent(std::string peerId_, std::string fingerprint_)
        : EventData("PeerRequestEvent"), peerId(std::move(peerId_)), fingerprint(std::move(fingerprint_)) {}
};

struct ReceiveRemoteDescriptionEvent : public EventData
{
    std::string peerId;
    rtc::Description description;

    ReceiveRemoteDescriptionEvent(std::string peerId_, rtc::Description description_)
        : EventData("ReceiveRemoteDescriptionEvent"), peerId(std::move(peerId_)), description(std::move(description_)) {}
};

struct ReceiveRemoteCandidateEvent : public EventData
{
    std::string peerId;
    rtc::Candidate candidate;

    ReceiveRemoteCandidateEvent(std::string peerId_, rtc::Candidate candidate)
        : EventData("ReceiveRemoteCandidateEvent"), peerId(std::move(peerId_)), candidate(std::move(candidate)) {}
};

struct SendLocalDescriptionEvent : public EventData
{
    std::string id;
    std::string type;
    std::string description;

    SendLocalDescriptionEvent(std::string id, std::string type, std::string description)
        : EventData("SendLocalDescriptionEvent"), id(std::move(id)), type(std::move(type)), description(std::move(description)) {}
};

struct SendLocalCandidateEvent : public EventData
{
    std::string id;
    std::string candidate;
    std::string mid;

    SendLocalCandidateEvent(std::string id, std::string candidate, std::string mid)
        : EventData("SendLocalCandidateEvent"), id(std::move(id)), candidate(std::move(candidate)), mid(std::move(mid)) {}
};

struct OnTextChannelEvent : public EventData
{
    std::string peerId;
    std::shared_ptr<rtc::Channel> textChannel;

    OnTextChannelEvent(std::string peerId, std::shared_ptr<rtc::Channel> textChannel)
        : EventData("OnTextChannelEvent"), peerId(std::move(peerId)), textChannel(std::move(textChannel)) {}
};

struct OnEventChannelEvent : public EventData
{
    std::string peerId;
    std::shared_ptr<rtc::Channel> eventChannel;

    OnEventChannelEvent(std::string peerId, std::shared_ptr<rtc::Channel> eventChannel)
        : EventData("OnEventChannelEvent"), peerId(std::move(peerId)), eventChannel(std::move(eventChannel)) {}
};

class LibDatachannelPeer;

struct OnNewPeerEvent : public EventData
{
    std::shared_ptr<LibDatachannelPeer> peer;

    explicit OnNewPeerEvent(std::shared_ptr<LibDatachannelPeer> peer)
        : EventData("OnNewPeerEvent"), peer(std::move(peer)) {}
};

struct ReceiveRemoteDescriptionByPeerEvent : public EventData
{
    std::string peerId;
    rtc::Description description;

    ReceiveRemoteDescriptionByPeerEvent(std::string peerId_, rtc::Description description_)
        : EventData("ReceiveRemoteDescriptionByPeerEvent"), peerId(std::move(peerId_)), description(std::move(description_)) {}
};

struct ReceiveRemoteCandidateByPeerEvent : public EventData
{
    std::string peerId;
    rtc::Candidate candidate;

    ReceiveRemoteCandidateByPeerEvent(std::string peerId_, rtc::Candidate candidate)
        : EventData("ReceiveRemoteCandidateByPeerEvent"), peerId(std::move(peerId_)), candidate(std::move(candidate)) {}
};

struct SendLocalDescriptionByPeerEvent : public EventData
{
    std::string id;
    std::string intermediatePeerId;
    std::string type;
    std::string description;

    SendLocalDescriptionByPeerEvent(std::string id, std::string type, std::string description)
        : EventData("SendLocalDescriptionByPeerEvent"), id(std::move(id)), type(std::move(type)), description(std::move(description)) {}
};

struct SendLocalCandidateByPeerEvent : public EventData
{
    std::string id;
    std::string intermediatePeerId;
    std::string candidate;
    std::string mid;

    SendLocalCandidateByPeerEvent(std::string id, std::string candidate, std::string mid)
        : EventData("SendLocalCandidateByPeerEvent"), id(std::move(id)), candidate(std::move(candidate)), mid(std::move(mid)) {}
};