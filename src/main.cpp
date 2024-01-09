#include <ChatApp.h>

#include <Controller/ChannelController.h>
#include <Controller/ChatController.h>
#include <Controller/ConfigController.h>
#include <Controller/LoginController.h>
#include <Model/ApplicationLogic/UserLogic.h>
#include <Model/DataAccess/LibDataChannelChatAPI.h>
#include <Model/DataAccess/NlohmannJsonLocalUsersAPI.h>
#include <Model/Models/ConfigFile.hpp>
#include <Model/Models/User.h>
#include <View/Backend/GLFWWindowManager.h>
#include <View/Backend/IWindow.h>
#include <View/GUI/ImGuiManager.hpp>
#include <View/Panels/ChannelPanel.h>
#include <View/Panels/ChatPanel.h>
#include <View/Panels/ConfigPanel.h>
#include <View/Panels/LoginPanel.h>
#include <View/Panels/UserInfoPanel.h>
#include <View/Views/ChannelView.h>
#include <View/Views/ChatView.h>
#include <View/Views/ConfigView.h>
#include <View/Views/LoginView.h>

// Main code
int main(int, char**)
{
    //Get the config data.
    //All the parts of the application that need data from the config file should
    // get it from the ConfigFile object before the actual application is running.
    //This object should be taken in the constructor of the class that needs it as a reference
    // and load needed values, but not keep a reference to it.
    ConfigFile config(config::CONFIG_FILE_NAME);

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
    auto configController = ConfigController(userLogic, commandManager);

    //init view layer
    auto chatView = ChatView();
    auto channelView = ChannelView();
    auto loginView = LoginView();
    auto configView = ConfigView();

    //init panels
    auto channelPanel = ChannelPanel(channelController);
    auto userInfoPanel = UserInfoPanel(channelController);
    auto chatPanel = ChatPanel(chatController);
    auto loginPanel = LoginPanel(loginController);
    auto configPanel = ConfigPanel(configController);

    chatView.AddPanel(chatPanel);
    channelView.AddPanel(channelPanel);
    channelView.AddPanel(userInfoPanel);
    loginView.AddPanel(loginPanel);
    configView.AddPanel(configPanel);

    //init renderer, window
    static const constexpr RendererAPI rendererApi = RendererAPI::OpenGL;
    GLFWWindowManager windowManager(rendererApi);
    ImGuiManager<GLFWWindowManager, rendererApi> guiManager(windowManager);

    //init app
    ChatApp app(windowManager, guiManager);
    app.AddView(channelView);
    app.AddView(chatView);
    app.AddView(loginView);
    app.AddView(configView);

    //Close the config file before running the application
    config.Close();

    app.Run();

    return 0;
}
