//
// Created by Jonathan Richard on 2024-01-05.
//

#pragma once

#include "MessageDispatcher.h"


#include <functional>
#include <vector>


enum class MessageType;
class MessageFilter {
private:
    std::vector<std::function<bool(const BaseMessage<MessageType>&)>> filters;

public:
    void AddFilter(const std::function<bool(const BaseMessage<MessageType>&)>& filter) {
        filters.push_back(filter);
    }

    bool ApplyFilters(const BaseMessage<MessageType>& message) {
        for (const auto& filter : filters) {
            if (!filter(message)) {
                return false;
            }
        }
        return true;
    }
};