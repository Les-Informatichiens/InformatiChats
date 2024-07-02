//
// Created by Jonathan Richard on 2024-01-10.
//

#include "RemoteUserMap.h"

void RemoteUserMap::Add(const std::shared_ptr<RemoteUser>& remoteUser)
{
    peerMap.emplace(remoteUser->GetFingerprint(), remoteUser);
}

void RemoteUserMap::Remove(const std::string& fingerprint)
{
    if (Contains(fingerprint))
    {
        peerMap.erase(fingerprint);
    }
}

std::shared_ptr<RemoteUser> RemoteUserMap::Get(const std::string& fingerprint)
{
    if (Contains(fingerprint))
    {
        return peerMap.at(fingerprint);
    }
    return nullptr;
}

bool RemoteUserMap::Contains(const std::string& fingerprint) const
{
    return peerMap.contains(fingerprint);
}

std::vector<std::shared_ptr<RemoteUser>> RemoteUserMap::GetAll() const
{
    std::vector<std::shared_ptr<RemoteUser>> result;
    result.reserve(peerMap.size());
    for (const auto& pair : peerMap)
    {
        result.push_back(pair.second);
    }
    return result;
}
