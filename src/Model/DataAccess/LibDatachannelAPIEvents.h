//
// Created by Jonathan Richard on 2023-09-17.
//

#pragma once

#include <Model/EventBus.h>
#include <rtc/rtc.hpp>
#include <utility>

struct PeerRequestEvent : public EventData
{
    std::string peerId;

    explicit PeerRequestEvent(std::string peerId_)
        : EventData("PeerRequestEvent"), peerId(std::move(peerId_)) {}
};

struct RemoteDescriptionEvent : public EventData
{
    std::string peerId;
    rtc::Description description;

    RemoteDescriptionEvent(std::string peerId_, rtc::Description description_)
        : EventData("RemoteDescriptionEvent"), peerId(std::move(peerId_)), description(std::move(description_)) {}
};

struct RemoteCandidateEvent : public EventData
{
    std::string peerId;
    rtc::Candidate candidate;

    RemoteCandidateEvent(std::string peerId_, rtc::Candidate candidate)
        : EventData("RemoteCandidateEvent"), peerId(std::move(peerId_)), candidate(std::move(candidate)) {}
};