//
// Created by Jonathan Richard on 2023-09-30.
//

#pragma once

#include <any>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>


enum struct ExchangeType
{
    Test,
    TextRequestExchange,
    AuthChallengeExchange,
    ContactRequestExchange,
    PublicKeyRequestExchange,
    ChannelRequestExchange,
    QueryClientsByFingerprintExchange,
    QueryDiscoverableExchange,
};

class ExchangeState
{
public:
    virtual ~ExchangeState() = default;

    template<typename T>
    constexpr bool Is()
    {
        static_assert(!std::is_abstract_v<T>, "This should not be a base type.");
        return dynamic_cast<T*>(this) != nullptr;
    }

    void setUserData(std::any userData) {
        userData_ = std::move(userData);
    }

    template <typename T>
    T getUserData() {
        try {
            return std::any_cast<T>(userData_);
        } catch (const std::bad_any_cast&) {
            throw;
        }
    }

private:
    std::any userData_;
};

template<typename T>
class ExchangeStateFactory
{
public:
    static constexpr std::unique_ptr<T> Make() { return std::make_unique<T>(); };
    static constexpr std::unique_ptr<T> Make(std::any data)
    {
        auto state = std::make_unique<T>();
        state->setUserData(std::move(data));
        return state;
    };
};

template<typename T>
class ExchangeStateObserver
{
public:
    using Callback = std::function<void(std::shared_ptr<T>)>;

    virtual ~ExchangeStateObserver() = default;
    virtual void onStateChange(std::shared_ptr<T> newState) = 0;
};

template<typename T>
struct DefaultExchangeStateObserver : ExchangeStateObserver<T>
{
    using Handler = typename ExchangeStateObserver<T>::Callback;
    explicit DefaultExchangeStateObserver(Handler&& handler) : handler_(std::forward<Handler>(handler)) {}

    void onStateChange(std::shared_ptr<T> newState) override
    {
        handler_(newState);
    }
    Handler handler_;
};

class Exchange
{
friend class ExchangeManager;

public:
    explicit Exchange(std::unique_ptr<ExchangeState> state_)
        : state(std::move(state_))
    {
    }

    virtual ~Exchange() = default;
    virtual bool SetState(std::shared_ptr<ExchangeState> stateId) = 0;
    virtual ExchangeType Type() = 0;

    template<typename T, typename Handler>
    void addObserver(Handler&& handler) {
        std::type_index index(typeid(T));
        using ObserverType = DefaultExchangeStateObserver<T>;
        if (auto it = observers.find(index); it != observers.end()) {
            it->second.emplace_back(std::make_shared<ObserverType>(std::forward<Handler>(handler)));
        } else {
            observers[index] = { std::make_shared<ObserverType>(std::forward<Handler>(handler)) };
        }
    }

    template<typename T>
    void notifyStateChange(std::shared_ptr<T> newState) {
        std::type_index index(typeid(*newState));
        auto it = observers.find(index);
        if (it != observers.end()) {
            for (auto& observer : it->second) {
                auto concreteObserver = std::static_pointer_cast<ExchangeStateObserver<T>>(observer);
                concreteObserver->onStateChange(newState);
            }
        }
    }

protected:
    bool StateResult(std::shared_ptr<ExchangeState>& newState_, bool success, bool complete = false)
    {
        if (success)
        {
            this->notifyStateChange(newState_);
            this->state = newState_;
            if (complete)
                this->onResult(true);
            return true;
        }
        this->onResult(false);
        return false;
    }

protected:
    std::shared_ptr<ExchangeState> state;
    std::function<void(bool)> onResult;

private:
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<void>>> observers;
};

class OfferTextRequestExchange : public Exchange
{
public:

    class AwaitingResponseState : public ExchangeState, public ExchangeStateFactory<AwaitingResponseState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    OfferTextRequestExchange()
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
        return ExchangeType::TextRequestExchange;
    }
};

class AnswerTextRequestExchange : public Exchange
{
public:
    class AwaitingAckState : public ExchangeState, public ExchangeStateFactory<AwaitingAckState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};


    AnswerTextRequestExchange()
        : Exchange(AwaitingAckState::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingAckState>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::TextRequestExchange;
    }
};

class EventExchangeWrapper
{
public:

    template<typename T>
    void Start()
    {
        this->exchange = std::make_shared<T>();
    }
    bool SetState(std::unique_ptr<ExchangeState> exchangeState)
    {
        return this->exchange->SetState(std::move(exchangeState));
    }
    bool SetState(std::unique_ptr<ExchangeState> exchangeState, std::any data)
    {
        if (this->exchange->SetState(std::move(exchangeState)))
        {
            this->data = std::move(data);
            return true;
        }
        return false;
    }

    ExchangeType Type()
    {
        return this->exchange->Type();
    }
    void OnSucceed(std::function<void()> callback);
    void OnFail(std::function<void()> callback);
    void OnForceReset(std::function<void()> callback);
    void OnStateChange(std::function<void(std::shared_ptr<ExchangeState>)> callback);

private:
    std::unique_ptr<Exchange> exchange;
    std::any data;

};

class OfferAuthChallengeRequest : public Exchange
{
public:

    class AwaitingChallengeAnswerState : public ExchangeState, public ExchangeStateFactory<AwaitingChallengeAnswerState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    OfferAuthChallengeRequest()
        : Exchange(AwaitingChallengeAnswerState::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingChallengeAnswerState>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::AuthChallengeExchange;
    }
};

class OfferContactRequest : public Exchange
{
public:

    class AwaitingContactRequestAnswerState : public ExchangeState, public ExchangeStateFactory<AwaitingContactRequestAnswerState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    OfferContactRequest()
        : Exchange(AwaitingContactRequestAnswerState::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingContactRequestAnswerState>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::ContactRequestExchange;
    }
};

class OfferPublicKeyRequest : public Exchange
{
public:

    class AwaitingPublicKeyRequestResponse : public ExchangeState, public ExchangeStateFactory<AwaitingPublicKeyRequestResponse> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    OfferPublicKeyRequest()
        : Exchange(AwaitingPublicKeyRequestResponse::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingPublicKeyRequestResponse>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::PublicKeyRequestExchange;
    }
};

class OfferChannelRequest : public Exchange
{
public:

    class AwaitingChannelRequestResponseState : public ExchangeState, public ExchangeStateFactory<AwaitingChannelRequestResponseState> {};
    class CompletedState : public ExchangeState, public ExchangeStateFactory<CompletedState> {};

    OfferChannelRequest()
        : Exchange(AwaitingChannelRequestResponseState::Make())
    {
    }

    bool SetState(std::shared_ptr<ExchangeState> newState_) override
    {
        if (this->state->Is<AwaitingChannelRequestResponseState>())
            return StateResult(newState_, newState_->Is<CompletedState>(), true);
        else
            return StateResult(newState_, false);
    }

    ExchangeType Type() override
    {
        return ExchangeType::ChannelRequestExchange;
;
    }
};

class ExchangeManager
{
public:
    template<typename T>
    std::pair<bool, std::shared_ptr<Exchange>> StartNewExchange(std::function<void(bool)> callback = nullptr)
    {
        static_assert(std::is_base_of_v<Exchange, T>);
        auto exchange = std::make_shared<T>();
        auto type = exchange->Type();
        exchange->onResult = [this, type](auto&&... args) { this->NotifyResult(type, std::forward<decltype(args)>(args)...); };
        return {this->exchanges.emplace(type, std::make_pair(exchange, std::move(callback))).second, exchange};
    }

    bool IsExchangeActive(ExchangeType exchangeType)
    {
        return this->exchanges.contains(exchangeType);
    }

    bool SetExchangeState(ExchangeType exchangeType, std::unique_ptr<ExchangeState> exchangeState)
    {
        auto exIt = this->exchanges.find(exchangeType);
        if (exIt != this->exchanges.end())
        {
            return exIt->second.first->SetState(std::move(exchangeState));
        }
        return false;
    }

    void EndExchange(ExchangeType exchangeType)
    {
        auto exIt = this->exchanges.find(exchangeType);
        if (exIt != this->exchanges.end())
        {
            this->exchanges.erase(exIt);
        }
    }

    void ForceFail(ExchangeType exchangeType)
    {
        this->NotifyResult(exchangeType, false);
    }

private:
    void NotifyResult(ExchangeType type, auto&&... args)
    {
        auto exIt = this->exchanges.find(type);
        if (exIt != this->exchanges.end())
        {
            auto callback = std::move(exIt->second.second);
            if (callback)
                callback(std::forward<decltype(args)>(args)...);
            this->exchanges.erase(exIt);
        }
    }

private:
    std::unordered_map<ExchangeType, std::pair<std::shared_ptr<Exchange>, std::function<void(bool)>>> exchanges{};
};
