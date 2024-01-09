//
// Created by Jonathan Richard on 2023-09-22.
//

#pragma once

#include <memory>
#include <unordered_map>

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

    template<typename... T, typename Handler>
    void RegisterHandler(Handler&& handler)
    {
        ((handlers_[T::opcode] = std::make_unique<MessageHandler<T, Handler>>(
                std::forward<Handler>(handler))), ...);
    }

    template<typename T, typename Handler>
    void RegisterHandler(Handler&& handler)
    {
        handlers_[T::opcode] = std::make_unique<MessageHandler<T, Handler>>(
                std::forward<Handler>(handler));
    }

    void Dispatch(const BaseMessage<TypeEnum>& message)
    {
        auto it = handlers_.find(message.GetOpcode());
        if (it != handlers_.end())
        {
            it->second->Handle(message);
        }
    }

private:
    struct BaseMessageHandler
    {
        virtual ~BaseMessageHandler() = default;
        virtual void Handle(const BaseMessage<TypeEnum>& message) = 0;
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
        Handler handler_;
    };

    std::unordered_map<TypeEnum, std::unique_ptr<BaseMessageHandler>> handlers_;
};
