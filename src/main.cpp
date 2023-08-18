#include "ChatApp.h"
#include "View/Views/ConnexionView.h"
#include "Controller/ConnexionController.h"
#include "View/Panels/ConnexionPanel.h"
#include <View/Panels/ChannelPanel.h>
#include <View/Panels/UserInfoPanel.h>
#include <View/Panels/ChatPanel.h>
#include <Controller/ChannelController.h>
#include <Controller/ChatController.h>
#include <View/Views/ChatView.h>
#include <View/Views/ChannelView.h>

// Main code
int main(int, char**)
{
    //init model
    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "51.79.86.30";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = { stunServer, stunServerPort, signalingServer, signalingServerPort };
    Chat chatClient(config);

    //init controller
    auto chatController = ChatController();
    auto channelController = ChannelController();
    auto connexionController = ConnexionController();

    //init view
    auto chatView = ChatView(chatController);
    auto channelView = ChannelView(channelController);
    auto connexionView = ConnexionView(connexionController);


    auto channelPanel = ChannelPanel(channelController);
    auto userInfoPanel = UserInfoPanel(channelController);
    auto chatPanel = ChatPanel(chatController);
    auto connexionPanel = ConnexionPanel(connexionController);

    chatView.AddPanel(chatPanel);
    channelView.AddPanel(channelPanel);
    channelView.AddPanel(userInfoPanel);
    connexionView.AddPanel(connexionPanel);

    //init app
    ChatApp app(chatClient);
    app.addView(channelView);
    app.addView(chatView);
    app.addView(connexionView);

    app.Run();

    return 0;
}