#include "ChatApp.h"
#include "View/Panels/ChannelPanel.h"
#include "View/Panels/UserInfoPanel.h"
#include "View/Panels/ChatPanel.h"
#include "Controller/ChannelController.h"
#include "Controller/ChatController.h"
#include "View/Views/ChatView.h"
#include "View/Views/ChannelView.h"

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
    ChatController chatController = ChatController();
    ChannelController channelController = ChannelController();

    //init view
    ChatView chatView = ChatView(chatController);
    ChannelView channelView = ChannelView(channelController);

    auto channelPanel = ChannelPanel(chatClient);
    auto userInfoPanel = UserInfoPanel(chatClient);
    auto chatPanel = ChatPanel(chatClient);

    chatView.AddPanel(chatPanel);
    channelView.AddPanel(channelPanel);
    channelView.AddPanel(userInfoPanel);

    //init app
    ChatApp app(chatClient);
    app.addView(channelView);
    app.addView(chatView);

    app.Run();

    return 0;
}