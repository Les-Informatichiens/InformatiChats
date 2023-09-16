#include <ChatApp.h>
#include <Controller/ChannelController.h>
#include <Controller/ChatController.h>
#include <Controller/LoginController.h>
#include <Model/ApplicationLogic/UserLogic.h>
#include <Model/DataAccess/LibDataChannelChatAPI.h>
#include <Model/DataAccess/NlohmannJsonLocalUsersAPI.h>
#include <Model/Models/User.h>
#include <View/Backend/GLFWWindowManager.h>
#include <View/Backend/IWindow.h>
#include <View/GUI/ImGuiManager.hpp>
#include <View/Panels/ChannelPanel.h>
#include <View/Panels/ChatPanel.h>
#include <View/Panels/LoginPanel.h>
#include <View/Panels/UserInfoPanel.h>
#include <View/Views/ChannelView.h>
#include <View/Views/ChatView.h>
#include <View/Views/LoginView.h>

// Main code
int main(int, char**)
{
    //init model layer
    User user{};
    auto chatAPI = LibDataChannelChatAPI();
    auto localUsersAPI = NlohmannJsonLocalUsersAPI();
    UserLogic userLogic{user, chatAPI, localUsersAPI};

    //init command manager
    CommandManager commandManager{};

    //init controller layer
    auto chatController = ChatController(userLogic, commandManager);
    auto channelController = ChannelController(userLogic, commandManager);
    auto loginController = LoginController(userLogic, commandManager);

    //init view layer
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

    //init renderer, window
    static const constexpr RendererAPI rendererApi = RendererAPI::OpenGL;
    GLFWWindowManager windowManager(rendererApi);
    ImGuiManager<GLFWWindowManager, rendererApi> guiManager(windowManager);

    //init app
    ChatApp app(windowManager, guiManager);
    app.AddView(channelView);
    app.AddView(chatView);
    app.AddView(loginView);

    app.Run();

    return 0;
}
