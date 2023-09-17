//
// Created by Jonathan Richard on 2023-09-17.
//

#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

// Base class for event data
struct EventData
{
    std::string eventType;

    explicit EventData(std::string eventType) : eventType(std::move(eventType)) {}
};

class EventBus
{
public:
    // Subscribe to an event with a callback function.
    void Subscribe(const std::string& eventName, std::function<void(const EventData&)> callback)
    {
        subscribers[eventName].push_back(callback);
    }

    // Publish an event with event data.
    template<typename T>
    void Publish(const T& eventData)
    {
        if (subscribers.find(eventData.eventType) != subscribers.end())
        {
            for (const auto& callback: subscribers[eventData.eventType])
            {
                callback(eventData);
            }
        }
    }

private:
    std::map<std::string, std::vector<std::function<void(const EventData&)>>> subscribers;
};
