//
// Created by Jonathan Richard on 2024-01-10.
//

#pragma once


#include "LocalUserData.h"
#include "Peer.h"
#include "util/crypto/Hashing.h"

#include <string>


class RemoteUser;
using OnUserOnlineCallback = std::function<void()>;
using OnUserOfflineCallback = std::function<void()>;

class RemoteUser
{
public:
    RemoteUser(std::string username, std::string fingerprint)
        : permanentUsername(std::move(username))
        , fingerprint(std::move(fingerprint))
        , profile()
    {
    }

    ~RemoteUser()
    {
        std::cout << "RemoteUser destroyed" << std::endl;
    }

    void RefreshProfile()
    {
        if (!this->IsOnline())
            return;

        this->activePeer->RequestProfile();
    }

    void SetActivePeer(std::shared_ptr<Peer> peer)
    {
        this->activePeer = std::move(peer);
        this->SetupActivePeer();
    }

    void SetProfile(const UserProfile& profile)
    {
        this->profile = profile;
    }

    void OnUserOnline(OnUserOnlineCallback cb)
    {
        this->onUserOnlineCb = std::move(cb);
    }

    void OnUserOffline(OnUserOfflineCallback cb)
    {
        this->onUserOfflineCb = std::move(cb);
    }

    [[nodiscard]] bool IsOnline() const
    {
        return this->activePeer && this->activePeer->IsConnected();
    }

    [[nodiscard]] std::string GetFingerprint() const
    {
        return this->fingerprint;
    }

    [[nodiscard]] const std::string& GetPermanentUsername() const
    {
        return this->permanentUsername;
    }

    [[nodiscard]] const std::string& GetPublicIdentificationKey() const
    {
        return "";
        // return publicIdentificationKey;
    }

    [[nodiscard]] const UserProfile& GetProfile() const
    {
        return this->profile;
    }

    [[nodiscard]] std::shared_ptr<Peer> GetActivePeer() const
    {
        return this->activePeer;
    }

private:
    void OnUserOnlineInternal()
    {
        if (this->onUserOnlineCb)
            this->onUserOnlineCb();
    }

    void OnUserOfflineInternal()
    {
        if (this->onUserOnlineCb)
            this->onUserOnlineCb();
    }

    void SetupActivePeer()
    {
        if (!this->activePeer)
            return;

        this->activePeer->OnConnected([this] {
            this->OnUserOnlineInternal();
        });
        this->activePeer->OnDisconnected([this] {
            this->OnUserOfflineInternal();
        });
    }

private:

    const std::string permanentUsername;
    const std::string fingerprint;
    UserProfile profile;

    std::shared_ptr<Peer> activePeer;

    // std::vector<std::pair<std::string, std::shared_ptr<Peer>>> potentialPeerList;

    OnUserOnlineCallback onUserOnlineCb;
    OnUserOfflineCallback onUserOfflineCb;

};