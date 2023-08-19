#include "User.h"

#include <utility>


User::User(std::string username_)
: username(std::move(username_))
{
}

std::string User::GetUsername() const
{
    return this->username;
}
