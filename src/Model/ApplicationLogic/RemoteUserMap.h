//
// Created by Jonathan Richard on 2024-01-04.
//

#pragma once

#include "Model/Models/RemoteUser.h"


#include <Model/Models/Peer.h>
#include <string>
#include <unordered_map>


class RemoteUserMap
{
public:
    RemoteUserMap() = default;

    void Add(const std::shared_ptr<RemoteUser>& remoteUser);
    void Remove(const std::string& fingerprint);
    std::shared_ptr<RemoteUser> Get(const std::string& fingerprint);

    [[nodiscard]] bool Contains(const std::string& fingerprint) const;

    [[nodiscard]] std::vector<std::shared_ptr<RemoteUser>> GetAll() const;

private:
    std::unordered_map<std::string, std::shared_ptr<RemoteUser>> peerMap;
};
