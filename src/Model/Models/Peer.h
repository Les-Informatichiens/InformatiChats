//
// Created by Jonathan Richard on 2023-09-30.
//

#pragma once

#include "ChatMessageInfo.h"
#include "Exchange.h"
#include "Model/DataAccess/IPeeringAPI.h"
#include "Model/Models/ConnectionState.h"
#include "util/crypto/RSAEncryption.h"

#include <promise-cpp/promise.hpp>
#include <utility>
#include <vector>

// using ChatHistory = std::vector<ChatMessageInfo>;
using OnValidationCallback = std::function<void()>;
using OnConnectedCallback = std::function<void()>;
using OnDisconnectedCallback = std::function<void()>;


class Peer
{
public:
    explicit Peer(IPeeringAPI& peeringAPI)
        : peeringAPI(peeringAPI)
    {
    }
    Peer(std::string peerId, std::string signalingId, std::string fingerprint, IPeeringAPI& peeringAPI)
        : peerId(std::move(peerId)), signalingId(std::move(signalingId)), fingerprint(std::move(fingerprint)), peeringAPI(peeringAPI)
    {
        this->SetupInternal();
    }

    ~Peer()
    {
        std::cout << "Peer destroyed" << std::endl;
        this->Disconnect();
    }

    void Connect(const std::string& signalingId)
    {
        if (this->IsConnected())
            return;

        this->signalingId = signalingId;
        this->peerId = this->peeringAPI.OpenPeerConnection(signalingId, nullptr);
        this->SetupInternal();
    }

    promise::Promise ConnectPromise(const std::string& signalingId)
    {
        promise::Promise promise;
        this->Connect(signalingId);
        this->OnConnected([promise] {
            promise.resolve();
        });
        return promise;
    }

    void OnValidation(OnValidationCallback callback)
    {
        this->onValidationCb = std::move(callback);
    }

    promise::Promise OnValidation()
    {
        promise::Promise promise;
        this->OnValidation([promise] {
            promise.resolve();
        });
        return promise;
    }

    void VerifyAuthenticity(const std::string& publicKey, const std::function<void(bool, std::string)>& callback)
    {
        auto [hasStarted, exchangePtr] = ongoingExchanges.StartNewExchange<OfferAuthChallengeRequest>([this](bool success) {
            if (!success)
            {
                this->peeringAPI.SendMessage(this->peerId, ResetExchange{ExchangeType::AuthChallengeExchange});
            }
            else
            {
                std::cout << "Exchange of type " << static_cast<int>(ExchangeType::AuthChallengeExchange) << " with " << peerId << " was successful" << std::endl;
            }
        });

        std::string originalToken = "banana";
        std::string encryptedToken = EncryptRSA(originalToken, publicKey);

        if (hasStarted)
        {
            exchangePtr->addObserver<OfferAuthChallengeRequest::CompletedState>([this, originalToken, publicKey, callback](const std::shared_ptr<ExchangeState>& newState) {
                auto decryptedToken = newState->getUserData<std::string>();
                if (decryptedToken == originalToken)
                {
                    this->verifiedPublicKey = publicKey;
                    this->authenticated = true;
                    // this->AddSystemLogToPeerChatHistory("Authentication successful.", {0.4f, 1.0f, 0.4f});
                    std::cout << "Authentication successful!" << std::endl;
                }
                else
                {
                    this->authenticated = false;
                    // this->AddSystemLogToPeerChatHistory("Authentication failed.", {1.0f, 0.4f, 0.4f});
                    std::cout << "Authentication failed!" << std::endl;
                }
                if (callback)
                    callback(decryptedToken == originalToken, publicKey);
            });
        }
        this->peeringAPI.SendMessage(peerId, AuthChallengeRequest{encryptedToken});
    }

    promise::Promise VerifyAuthenticity(const std::string& publicKey)
    {
        promise::Promise promise;
        this->VerifyAuthenticity(publicKey, [promise](bool success, std::string publicKey) {
            if (success)
                promise.resolve();
            else
                promise.reject();
        });
        return promise;
    }

    // void AddSystemLogToPeerChatHistory(const std::string& message, std::array<float, 3> color = {0.4, 1.0f, 0.4f})
    // {
    //     this->AddChatMessageToPeerChatHistory({message,
    //                                            duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
    //                                            "System",
    //                                            {"System",
    //                                             "System",
    //                                             "System",
    //                                             color}});
    // }
    //
    // void AddChatMessageToPeerChatHistory(const ChatMessageInfo& chatMessage)
    // {
    //     this->chatHistory.emplace_back(chatMessage);
    // }

    template<typename... Ts, typename Handler>
    void SubscribeEvents(Handler&& handler)
    {
        this->messageDispatcher.RegisterHandlers<Ts...>(std::forward<Handler>(handler));
    }

    template<typename T, typename Handler>
    void SubscribeEvent(Handler&& handler)
    {
        this->messageDispatcher.RegisterHandler<T, Handler>(std::forward<Handler>(handler));
    }

    template<typename T, typename Handler>
    void SubscribeEventOnce(Handler&& handler)
    {
        this->messageDispatcher.RegisterOneTimeHandler<T, Handler>(std::forward<Handler>(handler));
    }

    template<typename Handler>
    void SubscribeEventGeneric(Handler&& handler)
    {
        this->messageDispatcher.RegisterGenericHandler<Handler>(std::forward<Handler>(handler));
    }

    void UnsubscribeAll()
    {
        this->messageDispatcher.Clear();
    }

    void SendMessage(const BaseMessage<MessageType>& message)
    {
        if (!this->IsConnected())
            return;

        this->peeringAPI.SendMessage(this->peerId, message);
    }

    void OnConnected(std::function<void()> callback)
    {
        if (this->IsConnected())
            callback();
        else
        {
            this->onConnectedCb = std::move(callback);
        }
    }

    [[nodiscard]] bool IsConnected() const
    {
        return this->connectionState == ConnectionState::Connected;
    }

    void OnDisconnected(std::function<void()> callback)
    {
        if (!this->IsConnected())
            callback();
        else
        {
            this->onDisconnectedCb = std::move(callback);
        }
    }

    void Disconnect()
    {
        this->peeringAPI.ClosePeerConnection(this->peerId);
        this->connectionState = ConnectionState::Closed;
        this->signalingId = "";
        this->peerId = "";
    }

    void RequestProfile()
    {
        this->peeringAPI.SendMessage(this->peerId, UserProfileRequest{});
    }

    void RequestPublicKey(const std::function<void(std::string)>& onKeyReceived)
    {
        this->ongoingExchanges.EndExchange(ExchangeType::PublicKeyRequestExchange);
        if (auto [started, exchange] = this->ongoingExchanges.StartNewExchange<OfferPublicKeyRequest>(); started)
        {
            if (exchange)
            {
                exchange->addObserver<OfferPublicKeyRequest::CompletedState>([onKeyReceived](const std::shared_ptr<OfferPublicKeyRequest::CompletedState>& newState) {
                    onKeyReceived(newState->getUserData<std::string>());
                });
                this->SendMessage(PublicKeyRequest{});
            }
        }
    }

    promise::Promise RequestPublicKey()
    {
        promise::Promise promise = promise::newPromise();
        this->RequestPublicKey([promise](const std::string& publicKey) {
            promise.resolve(publicKey);
        });
        return promise;
    }

    void UpdateProfile(const UserProfile& profile)
    {
        this->profile = profile;
    }

    promise::Promise ValidateConnection()
    {
        // wait for the peer to be connected and then validate the connection
        return this->RequestPublicKey().then([this](const std::string& publicKey) {
            return this->VerifyAuthenticity(publicKey);
        }).then([] {
            return promise::resolve();
        }).fail([] {
            return promise::reject();
        });
    }

    void ValidateConnection(const std::function<void(bool, std::string)>& callback)
    {
        this->RequestPublicKey([this, callback](const std::string& publicKey) {
            this->VerifyAuthenticity(publicKey, callback);
        });
    }


private:
    void OnConnectInternal()
    {
        this->connectionState = ConnectionState::Connected;
        this->connected = true;
        if (this->onConnectedCb)
            this->onConnectedCb();
    }

    void OnMessageInternal(const BaseMessage<MessageType>& message)
    {
        this->messageDispatcher.Dispatch(message);
    }

    void OnStateChangeInternal(ConnectionState state)
    {
        this->connectionState = state;
        if (state == ConnectionState::Closed || state == ConnectionState::Failed || state == ConnectionState::Disconnected && this->connected)
        {
            this->connected = false;
            if (this->onDisconnectedCb)
                this->onDisconnectedCb();
        }

        if (state == ConnectionState::Connected)
        {
            // this->AddSystemLogToPeerChatHistory("Connection with " + peerId + " has been established.", {0.4f, 1.0f, 0.4f});
        }
        if (state == ConnectionState::Disconnected)
        {
            // this->AddSystemLogToPeerChatHistory(peerId + " has disconnected.", {1.0f, 0.4f, 0.4f});
        }
        if (state == ConnectionState::Failed)
        {
            // this->AddSystemLogToPeerChatHistory("Connection with " + peerId + " has failed.", {1.0f, 0.4f, 0.4f});
        }
        if (state == ConnectionState::Closed)
        {
            // this->AddSystemLogToPeerChatHistory("Connection with " + peerId + " has closed.", {1.0f, 0.4f, 0.4f});
        }
    }

    void OnValidateInternal()
    {
        if (this->onValidationCb)
        {
            this->onValidationCb();
        }
    }

    void SetupInternal()
    {
        this->peeringAPI.OnPeerMessage(this->peerId, [this](const BaseMessage<MessageType>& message) {
            this->OnMessageInternal(message);
        });
        this->peeringAPI.OnPeerConnected(this->peerId, [this] {
            this->OnValidateInternal();
            this->OnConnectInternal();
        });
        this->peeringAPI.OnPeerConnectionStateChange(this->peerId, [this](ConnectionState state) {
            this->OnStateChangeInternal(state);
        });
    }

private:
    std::string peerId;
    std::string signalingId;
    std::string fingerprint;

    std::string username;
    std::string verifiedPublicKey;

    std::string ipAddress;

    UserProfile profile{};

    size_t unreadMessageCount = 0;
    ConnectionState connectionState = ConnectionState::New;
    bool connected = false;

    bool authenticated = false;
    bool validated = false;

    // PeerData peerData{};
    // ChatHistory chatHistory;

    ExchangeManager ongoingExchanges;
    IPeeringAPI& peeringAPI;
    MessageDispatcher<MessageType> messageDispatcher{};

    OnValidationCallback onValidationCb;
    OnConnectedCallback onConnectedCb;
    OnDisconnectedCallback onDisconnectedCb;

public:
    void SetVerifiedPublicKey(const std::string& publicKey) { this->verifiedPublicKey = publicKey; }
    void SetUsername(const std::string& username) { this->username = username; }
    void SetIpAddress(const std::string& ipAddress) { this->ipAddress = ipAddress; }
    void SetFingerprint(const std::string& fingerprint) { this->fingerprint = fingerprint; }

    [[nodiscard]] const std::string& GetPeerId() const { return peerId; }
    [[nodiscard]] const std::string& GetUsername() const { return username; }
    [[nodiscard]] const std::string& GetVerfiedPublicKey() const { return verifiedPublicKey; }
    [[nodiscard]] const std::string& GetFingerprint() const { return fingerprint; }
    [[nodiscard]] const std::string& GetSignalingId() const { return signalingId; }
    [[nodiscard]] const std::string& GetIpAddress() const { return ipAddress; }
    [[nodiscard]] const UserProfile& GetProfile() const { return profile; }
    [[nodiscard]] size_t GetUnreadMessageCount() const { return unreadMessageCount; }
    [[nodiscard]] ConnectionState GetConnectionState() const { return connectionState; }
    [[nodiscard]] bool IsAuthenticated() const { return authenticated; }
    // [[nodiscard]] const ChatHistory& GetChatHistory() const { return chatHistory; }
    [[nodiscard]] ExchangeManager& GetOngoingExchanges() { return ongoingExchanges; }

private:
    friend class LocalUser;
    friend class UserLogic;
    friend class UserDataManager;
};
