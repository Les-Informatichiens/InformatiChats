//
// Created by Jonathan Richard on 2023-08-09.
//

#ifndef INFORMATICHATS_APPLICATION_H
#define INFORMATICHATS_APPLICATION_H
// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
#include <imgui.h>
//#include <backends/imgui_impl_glfw.h>
#include "imgui_impl_glfw_pixel.h"
//#include <backends/imgui_impl_opengl3.h>
#include "imgui_impl_opengl3_pixel.h"
#include <cstdio>

#include "Chat.h"

#include <cstdio>
#include <unordered_map>
#include <memory>

#include "Shaders.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#include "pxlui/ShaderProgram.h"
#include "pxlui/BatchRenderer.h"
#include "pxlui/GLShaderUtil.h"

#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif
#define IMGUI_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS


class Application
{
public:
    // TODO: add a constructor
    explicit Application(){

        const std::string stunServer = "stun.l.google.com";
        const std::string stunServerPort = "19302";
        const std::string signalingServer = "51.79.86.30";
        const std::string signalingServerPort = "51337";
        ConnectionConfig config = { stunServer, stunServerPort, signalingServer, signalingServerPort };
        chatClient = Chat(config);
        Init();
    };
    bool Init();
    void Update();
    void Uninit();

private:
    GLFWwindow* window{};

    Chat chatClient;
    std::string selectedChat;
    bool addNewChatPrompt{};

    // chat message space
    ExampleAppConsole console;
    bool consoleOpen{};

    struct PeerData
    {
        ImVector<char*> history;
        size_t unreadMessageCount;
    };
    // chat histories
    std::unordered_map<std::string, PeerData> historyMap;

    bool showDemoWindow{};
    const float resFactor = 0.5f;
    static constexpr int maxNameLength{32};
    PxlUI::ShaderProgram* ShaderProg;
    GLuint FramebufferTexture{};
    GLuint Framebuffer{};
    char InputBuf[256]{};
    ImVec4 clear_color;
};


#endif //INFORMATICHATS_APPLICATION_H
