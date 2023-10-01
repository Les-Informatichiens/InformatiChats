//
// Created by Jonathan Richard on 2023-09-30.
//

#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>


enum struct ExchangeType
{
    TextRequestExchange
};

class ExchangeState
{
public:
    virtual ~ExchangeState() = default;

};

template<typename T>
class ExchangeStateFactory
{
public:
    static std::unique_ptr<T> Make() { return std::make_unique<T>(); };
};

class Exchange
{
public:
    virtual ~Exchange() = default;
    virtual bool SetState(std::unique_ptr<ExchangeState> stateId) = 0;
    virtual ExchangeType Type() = 0;
};

class OfferTextRequestExchange : public Exchange
{
public:

    class AwaitingResponseState : public ExchangeState, public ExchangeStateFactory<AwaitingResponseState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    explicit OfferTextRequestExchange(std::function<void()> onCompletion)
        : state(AwaitingResponseState::Make()), onCompletion(onCompletion)
    {
    }

    bool SetState(std::unique_ptr<ExchangeState> newState_) override
    {
        auto result = [this, &newState_](bool success, bool complete = false) -> bool {
            if (success)
            {
                this->state = std::move(newState_);
                if (complete)
                    this->onCompletion();
                return true;
            }
            else
            {
                if (this->onFail)
                    this->onFail();
                return false;
            }
        };

        if (dynamic_cast<AwaitingResponseState*>(this->state.get()))
            return result(dynamic_cast<CompletedState*>(newState_.get()) != nullptr, true);
        else
            return result(false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::TextRequestExchange;
    }

private:
    std::unique_ptr<ExchangeState> state;
    std::function<void()> onCompletion;
    std::function<void()> onFail;
};

class AnswerTextRequestExchange : public Exchange
{
public:
    class AwaitingAckState : public ExchangeState, public ExchangeStateFactory<AwaitingAckState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};


    explicit AnswerTextRequestExchange(std::function<void()> onCompletion)
        : state(AwaitingAckState::Make()), onCompletion(onCompletion)
    {
    }

    bool SetState(std::unique_ptr<ExchangeState> newState_) override
    {
        auto result = [this, &newState_](bool success, bool complete = false) -> bool {
            if (success)
            {
                this->state = std::move(newState_);
                if (complete)
                    this->onCompletion();
                return true;
            }
            else
            {
                if (this->onFail)
                    this->onFail();
                return false;
            }
        };

        if (dynamic_cast<AwaitingAckState*>(this->state.get()))
            return result(dynamic_cast<CompletedState*>(newState_.get()) != nullptr, true);
        else
            return result(false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::TextRequestExchange;
    }

private:
    std::unique_ptr<ExchangeState> state;
    std::function<void()> onCompletion;
    std::function<void()> onFail;
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

    bool SetExchangeState(ExchangeType exchangeType, std::unique_ptr<ExchangeState> exchangeState)
    {
        auto exIt = this->exchanges.find(exchangeType);
        if (exIt != this->exchanges.end())
        {
            return exIt->second->SetState(std::move(exchangeState));
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
