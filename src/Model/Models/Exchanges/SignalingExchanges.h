//
// Created by Jonathan Richard on 2024-01-01.
//

#pragma once
#include "Model/Models/Exchange.h"


class QueryClientsByFingerprint : public Exchange
{
public:

    class AwaitingResponseState : public ExchangeState, public ExchangeStateFactory<AwaitingResponseState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    QueryClientsByFingerprint()
        : Exchange(AwaitingResponseState::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingResponseState>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::QueryClientsByFingerprintExchange;
    }
};

class QueryDiscoverable : public Exchange
{
public:

    class AwaitingResponseState : public ExchangeState, public ExchangeStateFactory<AwaitingResponseState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    QueryDiscoverable()
        : Exchange(AwaitingResponseState::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingResponseState>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::QueryDiscoverableExchange;
    }
};
