cmake_minimum_required(VERSION 3.30)
project(InformatiChats VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/View/imgui_impl_opengl3_loader.h
        src/View/imgui_impl_opengl3_pixel.cpp
        src/View/imgui_impl_opengl3_pixel.h
        src/View/pxlui/BatchRenderer.cpp
        src/View/pxlui/BatchRenderer.h
        src/View/pxlui/GLShaderUtil.cpp
        src/View/pxlui/GLShaderUtil.h
        src/View/pxlui/ShaderProgram.cpp
        src/View/pxlui/ShaderProgram.h
        src/View/imgui_impl_glfw_pixel.cpp
        src/View/imgui_impl_glfw_pixel.h
        src/View/Shaders.h
        src/ChatApp.cpp
        src/ChatApp.h
        src/View/Views/IView.h
        src/View/Panels/IPanel.h
        src/View/Panels/ChatPanel.cpp
        src/View/Panels/ChatPanel.h
        src/View/Panels/ChannelPanel.cpp
        src/View/Panels/ChannelPanel.h
        src/View/Panels/VideoPanel.cpp
        src/View/Panels/VideoPanel.h
        src/View/Panels/UserInfoPanel.cpp
        src/View/Panels/UserInfoPanel.h
        src/Controller/ChatController.cpp
        src/Controller/ChatController.h
        src/View/Views/ChannelView.cpp
        src/View/Views/ChannelView.h
        src/View/Views/ChatView.cpp
        src/View/Views/ChatView.h
        src/Controller/ChannelController.cpp
        src/Controller/ChannelController.h
        src/Controller/ViewModels/ChannelViewModel.h
        src/Controller/IChannelController.h
        src/Controller/IChatController.h
        src/Controller/ViewModels/ChatViewModel.h
        src/View/Views/LoginView.cpp
        src/View/Views/LoginView.h
        src/Controller/LoginController.cpp
        src/Controller/LoginController.h
        src/Controller/ILoginController.h
        src/View/Panels/LoginPanel.cpp
        src/View/Panels/LoginPanel.h
        src/Model/ApplicationLogic/UserLogic.cpp
        src/Model/ApplicationLogic/UserLogic.h
        src/Controller/ViewModels/LoginViewModel.h
        src/Model/DataAccess/IChatAPI.h
        src/Model/DataAccess/LibDataChannelChatAPI.cpp
        src/Model/DataAccess/LibDataChannelChatAPI.h
        src/Model/Models/User.h
        src/Model/Models/PeerData.h
        src/View/Backend/IWindow.h
        src/View/GUI/IGUIManager.h
        src/View/GUI/ImGuiManager.hpp
        src/View/Backend/GLFWWindowManager.cpp
        src/View/Backend/GLFWWindowManager.h
        src/util/string_util.cpp
        src/util/string_util.h
        src/Model/BinarySerializer.h
        src/Model/Models/ChatMessage.h
        src/util/crypto/Keygen.cpp
        src/util/crypto/Keygen.h
        src/util/crypto/AESEncryption.h
        src/util/crypto/AESEncryption.cpp
        src/util/crypto/RSAEncryption.cpp
        src/util/crypto/RSAEncryption.h
        src/util/crypto/CryptoTest.h
        src/Model/Models/UserData.h
        src/util/Base64.h
        src/Model/DataAccess/ILocalUsersAPI.h
        src/Model/Models/LocalUsers.h
        src/Model/DataAccess/NlohmannJsonLocalUsersAPI.cpp
        src/Model/DataAccess/NlohmannJsonLocalUsersAPI.h
        src/Model/ApplicationLogic/Command/ICommand.h
        src/Model/ApplicationLogic/Command/CommandManager.cpp
        src/Model/ApplicationLogic/Command/CommandManager.h
        src/Model/ApplicationLogic/Command/SetSelectedPeerIdCommand.cpp
        src/Model/ApplicationLogic/Command/SetSelectedPeerIdCommand.h
        src/Controller/IConfigController.h
        src/Controller/ConfigController.cpp
        src/Controller/ConfigController.h
        src/View/Views/ConfigView.cpp
        src/View/Views/ConfigView.h
        src/View/Panels/ConfigPanel.cpp
        src/View/Panels/ConfigPanel.h
        src/util/VariantUtil.h
        src/Model/ApplicationLogic/Command/SaveNewConfigValueCommand.cpp
        src/Model/ApplicationLogic/Command/SaveNewConfigValueCommand.h
        src/Model/Models/ConfigRefPair.h
        src/Model/Models/ConfigFile.hpp
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        src
)

if (MSVC)
    # Set the app as solution startup project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    # Parallel c++ compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif ()

# Dependencies ========================================================================================
include(cmake/CPM.cmake)

# LibDataChannel ======================================================================================
if (NOT TARGET LibDataChannel::LibDataChannelStatic)
    CPMFindPackage(
            NAME "LibDataChannel"
            VERSION "0.21.2"
            GITHUB_REPOSITORY "paullouisageneau/libdatachannel"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE LibDataChannel::LibDataChannelStatic)
# =====================================================================================================

# nlohmann-json =======================================================================================
if (NOT TARGET nlohmann_json::nlohmann_json)
    CPMFindPackage(
            NAME "nlohmann_json"
            VERSION "3.11.3"
            GITHUB_REPOSITORY "nlohmann/json"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
# =====================================================================================================

# openssl =============================================================================================
find_package(OpenSSL REQUIRED)
if (TARGET OpenSSL::Crypto)
    message(STATUS "Linking OpenSSL::Crypto")
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)
endif ()
# =====================================================================================================

# glm =================================================================================================
CPMAddPackage(
        NAME "glm"
        GITHUB_REPOSITORY "g-truc/glm"
        GIT_TAG "1.0.1"
)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
# =====================================================================================================

# glew ================================================================================================
if (NOT TARGET libglew_static)
    CPMAddPackage(
            NAME "GLEW"
            GITHUB_REPOSITORY "Perlmint/glew-cmake"
            GIT_TAG "glew-cmake-2.2.0"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
# =====================================================================================================

# imgui ===============================================================================================
if (NOT TARGET imgui::imgui)
    CPMFindPackage(
            NAME "imgui"
            GITHUB_REPOSITORY "Les-Informatichiens/imgui-cmake"
            GIT_TAG "v1.90.9-docking-cmake"
            OPTIONS
            "IMGUI_BUILD_OPENGL3_BINDING ON"
            "IMGUI_BUILD_GLFW_BINDING ON"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
# =====================================================================================================

# zpp_bits ============================================================================================
CPMAddPackage(
        NAME "zpp_bits"
        GITHUB_REPOSITORY "eyalz800/zpp_bits"
        VERSION "4.4.23"
)
if (zpp_bits_ADDED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${zpp_bits_SOURCE_DIR})
endif ()
# =====================================================================================================

# tr-and-slate-or =====================================================================================
if (NOT TARGET trandslateor)
    CPMFindPackage(
            NAME "trandslateor"
            GITHUB_REPOSITORY "ThierryCantin-Demers/tr-and-slate-or"
            GIT_TAG "v0.1.1"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE trandslateor)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/translations")
    message(STATUS "Copying translations to build directory")

    # The translations directory is copied to the build directory so that the application can find them since it needs
    # to be at the same level as the executable (with the base path).
    # That means that to add translations, you need to modify the translations directory that is directly in the repo
    # and not the one in the build directory. Otherwise, the changes will be overwritten and lost on cmake build.
    file(COPY translations DESTINATION "${CMAKE_BINARY_DIR}")
endif ()
# =====================================================================================================

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
    message(STATUS "Copying res to build directory")
    file(COPY res DESTINATION "${CMAKE_BINARY_DIR}")
endif ()

# Compile definitions ==================================================================================
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __RELEASE__ PRIVATE __NDEBUG__)
endif ()
# =====================================================================================================
