#include "ChatApp.h"
#include "Controller/LoginController.h"
#include "View/Panels/LoginPanel.h"
#include "View/Views/LoginView.h"
#include <Controller/ChannelController.h>
#include <Controller/ChatController.h>
#include <View/Panels/ChannelPanel.h>
#include <View/Panels/ChatPanel.h>
#include <View/Panels/UserInfoPanel.h>
#include <View/Views/ChannelView.h>
#include <View/Views/ChatView.h>

// Main code
int main(int, char **)
{
    //init model
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "51.79.86.30";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = {stunServer, stunServerPort, signalingServer, signalingServerPort};
    Chat chatClient(config);

    //init controller
    auto chatController = ChatController();
    auto channelController = ChannelController();
    auto loginController = LoginController();

    //init view
    auto chatView = ChatView(chatController);
    auto channelView = ChannelView(channelController);
    auto loginView = LoginView(loginController);


    auto channelPanel = ChannelPanel(channelController);
    auto userInfoPanel = UserInfoPanel(channelController);
    auto chatPanel = ChatPanel(chatController);
    auto loginPanel = LoginPanel(loginController);

    chatView.AddPanel(chatPanel);
    channelView.AddPanel(channelPanel);
    channelView.AddPanel(userInfoPanel);
    loginView.AddPanel(loginPanel);

    //init app
    ChatApp app(chatClient);
    app.addView(channelView);
    app.addView(chatView);
    app.addView(loginView);

    app.Run();

    return 0;
}