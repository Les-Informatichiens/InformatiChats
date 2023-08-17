#include "Application.h"
#include "View/View.h"
#include "View/Panels/ChannelPanel.h"
#include "View/Panels/UserInfoPanel.h"
#include "View/Panels/ChatPanel.h"

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

    //init view
    View view = View();
    auto channelPanel = ChannelPanel(chatClient);
    auto userInfoPanel = UserInfoPanel(chatClient);
    auto chatPanel = ChatPanel(chatClient);

    view.AddPanel(channelPanel);
    view.AddPanel(userInfoPanel);
    view.AddPanel(chatPanel);

    //injection des dependances dans l'app
    Application app(chatClient, view);

    app.Run();

    return 0;
}