//
// Created by Jean on 8/18/2023.
//

#pragma once

#include <Controller/ViewModels/ChatViewModel.h>

#include <string>

class IChatController
{
public:
    virtual ChatViewModel GetViewModel(const std::string& selectedChannel) = 0;

    virtual void SendMessage(const std::string& destinationChannel, const std::string& message) = 0;
    virtual void AddPeerAsContact(const std::string& peerId) = 0;
    virtual void RemovePeerFromContacts(const std::string& peerId) = 0;
    virtual void AddUserToChannel(const std::string& fingerprint, const std::string& channelUuid) = 0;
    virtual ChannelDTO GetChannelDTO(const std::string& channelUuid) = 0;

};
