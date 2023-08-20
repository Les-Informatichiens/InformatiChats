#include "ChatApp.h"

#include <Model/ChatClient.h>
#include <Model/User.h>
#include <Model/Model.h>

#include <Controller/ChannelController.h>
#include <Controller/ChatController.h>
#include <Controller/LoginController.h>
#include <View/Panels/ChannelPanel.h>
#include <View/Panels/ChatPanel.h>
#include <View/Panels/LoginPanel.h>
#include <View/Panels/UserInfoPanel.h>
#include <View/Views/ChannelView.h>
#include <View/Views/ChatView.h>
#include <View/Views/LoginView.h>


// Main code
int main(int, char **)
{
    //init model
    User user{};
    ChatClient chatClient{};

    Model model = Model(user, chatClient);

    //init controller
    auto chatController = ChatController(model);
    auto channelController = ChannelController(model);
    auto loginController = LoginController(model);

    //init view
    auto chatView = ChatView(chatController);
    auto channelView = ChannelView(channelController);
    auto loginView = LoginView(loginController);

    //init panels
    auto channelPanel = ChannelPanel(channelController);
    auto userInfoPanel = UserInfoPanel(channelController);
    auto chatPanel = ChatPanel(chatController);
    auto loginPanel = LoginPanel(loginController);

    chatView.AddPanel(chatPanel);
    channelView.AddPanel(channelPanel);
    channelView.AddPanel(userInfoPanel);
    loginView.AddPanel(loginPanel);

    //init app
    ChatApp app;
    app.AddView(channelView);
    app.AddView(chatView);
    app.AddView(loginView);

    app.Run();

    return 0;
}
