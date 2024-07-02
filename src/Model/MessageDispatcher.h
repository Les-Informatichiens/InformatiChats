//
// Created by Jonathan Richard on 2023-09-22.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include <iostream>

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

template<typename TypeEnum>
class BaseMessage
{
public:
    virtual ~BaseMessage() = default;
    virtual TypeEnum GetOpcode() const = 0;
    virtual std::vector<std::byte> Serialize() const = 0;
    virtual void Deserialize(const std::vector<std::byte>& data) const = 0;
};

template<typename TypeEnum>
class MessageDispatcher
{
public:

    template<typename Handler>
    void RegisterGenericHandler(Handler&& handler)
    {
        genericHandlers_.emplace_back(std::make_unique<GenericMessageHandler<Handler>>(
            std::move(handler)));
    }

    template<typename... T, typename Handler>
    void RegisterHandlers(Handler&& handler)
    {
        ((handlers_[T::opcode] = std::make_unique<MessageHandler<T, Handler>>(
                std::move(handler))), ...);
    }

    template<typename T, typename Handler>
    void RegisterHandler(Handler&& handler)
    {
        handlers_[T::opcode] = std::make_unique<MessageHandler<T, Handler>>(
                std::move(handler));

    }

    template<typename T, typename Handler>
    void RegisterOneTimeHandler(Handler&& handler)
    {
        handlers_[T::opcode] = std::make_unique<OneTimeMessageHandler<T, Handler>>(
                std::move(handler));
    }

    void Dispatch(const BaseMessage<TypeEnum>& message)
    {
        std::unique_lock<std::mutex> lock(dispatchMutex);  // Lock before dispatching
        auto it = handlers_.find(message.GetOpcode());
        if (it != handlers_.end())
        {
            try {
                it->second->Handle(message);
            } catch (const std::exception &e) {
                std::cerr << "Uncaught exception in callback: " << e.what() << std::endl;
            }
            if (it->second->IsExpired())
            {
                handlers_.erase(it);
            }
        }

        for (const auto& handler : genericHandlers_)
        {
            try {
                handler->Handle(message);
            } catch (const std::exception &e) {
                std::cerr << "Uncaught exception in callback: " << e.what() << std::endl;
            }
        }
        std::erase_if(genericHandlers_, [](const auto& handler) {
            return handler->IsExpired();
        });

        lock.unlock();
    }

    void Clear()
    {
        std::unique_lock<std::mutex> lock(dispatchMutex);  // Lock before dispatching
        handlers_.clear();
        genericHandlers_.clear();
        lock.unlock();
    }

private:
    struct BaseMessageHandler
    {
        virtual ~BaseMessageHandler() = default;
        virtual void Handle(const BaseMessage<TypeEnum>& message) = 0;
        virtual bool IsExpired() = 0;
    };

    template<typename T, typename Handler>
    struct MessageHandler : BaseMessageHandler
    {
        explicit MessageHandler(Handler&& handler) : handler_(std::forward<Handler>(handler)) {}

        void Handle(const BaseMessage<TypeEnum>& message) override
        {
            const T& typedMessage = dynamic_cast<const T&>(message);
            handler_(typedMessage);
        }
        bool IsExpired() override
        {
            return false;
        }
        Handler handler_;
    };

    template<typename Handler>
    struct GenericMessageHandler : BaseMessageHandler
    {
        explicit GenericMessageHandler(Handler&& handler) : handler_(std::forward<Handler>(handler)) {}

        void Handle(const BaseMessage<TypeEnum>& message) override
        {
            handler_(message);
        }
        bool IsExpired() override
        {
            return false;
        }

        Handler handler_;
    };

    template<typename T, typename Handler>
    struct OneTimeMessageHandler : BaseMessageHandler
    {
        explicit OneTimeMessageHandler(Handler&& handler) : handler_(std::forward<Handler>(handler)) {}

        void Handle(const BaseMessage<TypeEnum>& message) override
        {
            const T& typedMessage = dynamic_cast<const T&>(message);
            handler_(typedMessage);
            hasExecuted = true;
        }
        bool IsExpired() override
        {
            return hasExecuted;
        }

        bool hasExecuted = false;
        Handler handler_;
    };

    std::unordered_map<TypeEnum, std::shared_ptr<BaseMessageHandler>> handlers_;
    std::vector<std::shared_ptr<BaseMessageHandler>> genericHandlers_;
    std::mutex dispatchMutex;
};
