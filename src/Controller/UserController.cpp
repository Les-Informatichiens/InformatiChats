//
// Created by Jonathan Richard on 2023-12-29.
//

#include "UserController.h"

UserViewModel UserController::GetViewModel()
{
    const UserDataManager& userDataManager = this->userLogic.GetUserDataManager();
    return {
        userDataManager.GetUsername(),
        userDataManager.GetUserProfile().displayName,
        userDataManager.GetUserProfile().description,
        userDataManager.GetUserProfile().status,
        userDataManager.GetUserProfile().nameColor
    };
}

void UserController::UpdateUserDisplayName(const std::string& displayName)
{
    const UserDataManager& userDataManager = this->userLogic.GetUserDataManager();
    UserProfile currentProfile = userDataManager.GetUserProfile();
    currentProfile.displayName = displayName;
    this->userLogic.UpdateUserProfile(currentProfile);
}

void UserController::UpdateUserDescription(const std::string& description)
{
    const UserDataManager& userDataManager = this->userLogic.GetUserDataManager();
    UserProfile currentProfile = userDataManager.GetUserProfile();
    currentProfile.description = description;
    this->userLogic.UpdateUserProfile(currentProfile);
}

void UserController::UpdateUserStatus(const std::string& status)
{
    const UserDataManager& userDataManager = this->userLogic.GetUserDataManager();
    UserProfile currentProfile = userDataManager.GetUserProfile();
    currentProfile.status = status;
    this->userLogic.UpdateUserProfile(currentProfile);
}

void UserController::UpdateUserNameColor(const std::array<float, 3>& color) const
{
    const UserDataManager& userDataManager = this->userLogic.GetUserDataManager();
    UserProfile currentProfile = userDataManager.GetUserProfile();
    currentProfile.nameColor[0] = color[0];
    currentProfile.nameColor[1] = color[1];
    currentProfile.nameColor[2] = color[2];
    this->userLogic.UpdateUserProfile(currentProfile);
}