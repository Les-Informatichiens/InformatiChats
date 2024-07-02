#include "Model/DataAccess/libdatachannel/LibDatachannelConnectionAPI.h"
#include "Model/DataAccess/libdatachannel/LibDatachannelPeeringAPI.h"
#include "Model/DataAccess/libdatachannel/LibDatachannelState.h"
#include "Model/DataAccess/libdatachannel/LibDatachannelTextChatAPI.h"
#include "util/crypto/CryptoTest.h"
#include <ChatApp.h>
#include <Controller/ChannelController.h>
#include <Controller/ChatController.h>
#include <Controller/LoginController.h>
#include <Controller/UserController.h>
#include <Model/ApplicationLogic/UserLogic.h>
#include <Model/DataAccess/NlohmannJsonLocalUsersAPI.h>
#include <Model/Models/LocalUser.h>
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


// #include <sol/sol.hpp>
// #include <sol/assert.hpp>

// Main code
int main(int, char**)
{
    //
    // sol::state lua;
    // lua.open_libraries(sol::lib::base);
    // auto result = lua.script_file("myscript.lua", sol::script_pass_on_error);
    // if (!result.valid())
    // {
    //     sol::error err = result;
    //     std::cerr << err.what() << std::endl;
    // }
    // auto func = lua.get<sol::protected_function>("update");
    //
    // // run a script, get the result
    // int value = lua.script("return 54");
    // sol_c_assert(value == 54);

//
//    // Your exists class
//    class Cat
//    {
//    public:
//        Cat() = default;
//        explicit Cat(std::string a): name(std::move(a)) {};
//        virtual ~Cat() = default;
//    public:
//        void setName(const std::string& name_) { name = name_; };
//        const std::string& getName() const { return name; };
//        void eat(const std::list<std::string>& foods) {
//                std::cout << getName() << " has eaten " << foods.size() << " foods" << std::endl;
//        };
//        static void speak(const std::string& w) {
//                std::cout << "Cat has" << " said: " << w << std::endl;
//        };
//        //...
//    private:
//        std::string name;
//        //...
//    };
//
//
//    lua_State * L = luaL_newstate(); // create and init lua
//
//    // To export it:
//    LuaClass<Cat> luaCat(L, "AwesomeCat");
//    luaCat.ctor<std::string>();
//    luaCat.fun("setName", &Cat::setName);
//    luaCat.fun("getName", &Cat::getName);
//    luaCat.fun("eat", &Cat::eat);
//    // static mmember fuction was exported as Lua class member fuction.
//    // from Lua, call it as same as other member fuctions.
//    luaCat.fun("speak", &Cat::speak);
//    luaCat.def("tag", "Cat");
//
//    // Load and execute Lua code
//    luaL_openlibs(L); // Load standard Lua libraries
//    luaL_dofile(L, "myscript.lua"); // Execute a Lua script
//
//    // Close the LuaJIT state when done
//    lua_close(L);

    //init model layer
    auto libdatachannelState = LibDatachannelState();
    auto libDatachannelEventBus = EventBus();
    auto connectionAPI = LibDatachannelConnectionAPI(libdatachannelState, libDatachannelEventBus);
    auto textChatAPI = LibDatachannelTextChatAPI(libdatachannelState, libDatachannelEventBus);
    auto peeringAPI = LibDatachannelPeeringAPI(libdatachannelState, libDatachannelEventBus);
    auto localUsersAPI = NlohmannJsonLocalUsersAPI();

    LocalUser user{};
    UserDataManager userDataManager(user, localUsersAPI);
    UserLogic userLogic{userDataManager, connectionAPI, peeringAPI, textChatAPI, localUsersAPI};

    //init command manager
    CommandManager commandManager{};

    //init controller layer
    auto chatController = ChatController(userLogic, commandManager);
    auto channelController = ChannelController(userLogic, commandManager);
    auto loginController = LoginController(userLogic, commandManager);
    auto userController = UserController(userLogic);

    //init view layer
    auto chatView = ChatView(chatController);
    auto channelView = ChannelView(channelController);
    auto loginView = LoginView(loginController);

    auto viewEventBus = EventBus();

    //init panels
    auto channelPanel = ChannelPanel(channelController, viewEventBus);
    auto userInfoPanel = UserInfoPanel(userController);
    auto chatPanel = ChatPanel(chatController, viewEventBus);
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

    app.test = [&libdatachannelState, &textChatAPI, &peeringAPI] {
        ImGui::Begin("Test");
        // func();
        for (const auto& peerConnection: libdatachannelState.peerMap)
        {
            ImGui::Separator();
            ImGui::Text("%s", peerConnection.first.c_str());
            ImGui::SameLine();
            ImGui::Text("%d", peerConnection.second->State());
            auto textchannelid = textChatAPI.textChannelMap.contains(peerConnection.first) ? (textChatAPI.textChannelMap.at(peerConnection.first) ? textChatAPI.textChannelMap.at(peerConnection.first)->isOpen() : -1) : -1;
            ImGui::Text("TChannel: %d", textchannelid);
            auto channelid = libdatachannelState.peerMap.contains(peerConnection.first) ? libdatachannelState.peerMap.at(peerConnection.first)->IsConnected() : -1;
            ImGui::Text("Channel: %d", channelid);
        }


        ImGui::End();
    };
    app.Run();

    return 0;
}
