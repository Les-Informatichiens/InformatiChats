//
// Created by Jonathan Richard on 2023-09-30.
//

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>


enum struct ExchangeType
{
    TextRequestExchange
};

class Exchange
{
public:
    virtual ~Exchange() = default;
    virtual bool SetState(int16_t stateId) = 0;
    virtual ExchangeType Type() = 0;
};

class TextRequestExchange : public Exchange
{
public:
    enum State
    {
        None,
        AwaitingResponse,
        AwaitingAck,
        Completed
    };

    explicit TextRequestExchange(State startingState, std::function<void()> onCompletion)
        : state(startingState), onCompletion(onCompletion)
    {
    }
    
    bool SetState(int16_t newState_) override
    {
        State newState = static_cast<State>(newState_);
        switch (this->state)
        {
            case None:
            {
                switch (newState)
                {
                    case None:
                        return false;
                    case AwaitingResponse:
                        this->state = newState;
                        return true;
                    case AwaitingAck:
                        this->state = newState;
                        return true;
                    case Completed:
                        return false;
                }
            }
            case AwaitingResponse:
            {
                switch (newState)
                {
                    case None:
                        return false;
                    case AwaitingResponse:
                        return false;
                    case AwaitingAck:
                        return false;
                    case Completed:
                    {
                        this->state = newState;
                        onCompletion();
                        return true;
                    }
                }
            }
            case AwaitingAck:
            {
                switch (newState)
                {
                    case None:
                        return false;
                    case AwaitingResponse:
                        return false;
                    case AwaitingAck:
                        return false;
                    case Completed:
                    {
                        this->state = newState;
                        onCompletion();
                        return true;
                    }
                }
            }
            case Completed:
            {
                return false;
            }
        }
    }

    ExchangeType Type() override
    {
        return ExchangeType::TextRequestExchange;
    }

private:
    State state;
    std::function<void()> onCompletion;
};


class OngoingExchanges
{
public:
    bool StartNewExchange(std::shared_ptr<Exchange>&& exchange)
    {
        return this->exchanges.emplace(exchange->Type(), std::move(exchange)).second;
    }

    bool IsExchangeStarted(ExchangeType exchangeType)
    {
        return this->exchanges.contains(exchangeType);
    }

    bool SetExchangeState(ExchangeType exchangeType, int16_t exchangeState)
    {
        auto exIt = this->exchanges.find(exchangeType);
        if (exIt != this->exchanges.end())
        {
            return exIt->second->SetState(exchangeState);
        }
        else
        {
            return false;
        }
    }

    void EndExchange(ExchangeType exchangeType)
    {
        auto exIt = this->exchanges.find(exchangeType);
        if (exIt != this->exchanges.end())
        {
            this->exchanges.erase(exIt);
        }
    }

private:
    std::unordered_map<ExchangeType, std::shared_ptr<Exchange>> exchanges;
};