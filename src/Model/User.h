#pragma once

#include "ChatClient.h"


class User
{
public:
    explicit User(std::string username);

    //void SetUsername(std::string newUsername);
    [[nodiscard]] std::string GetUsername() const;

private:
    std::string username;
};
