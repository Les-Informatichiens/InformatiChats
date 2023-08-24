//
// Created by Jonathan Richard on 2023-08-09.
//

#pragma once

// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
#include <imgui.h>
//#include <backends/imgui_impl_glfw.h>
#include "View/imgui_impl_glfw_pixel.h"
//#include <backends/imgui_impl_opengl3.h>
#include "View/imgui_impl_opengl3_pixel.h"
#include "View/Shaders.h"

#include <cstdio>
#include <memory>
#include <unordered_map>
#include <vector>


#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#else

#include <GL/glew.h>
#include <View/Views/IView.h>
#include <View/pxlui/BatchRenderer.h>
#include <View/pxlui/GLShaderUtil.h>
#include <View/pxlui/ShaderProgram.h>

#endif

#include <GLFW/glfw3.h>// Will drag system OpenGL headers

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

struct DisplaySize
{
    int width;
    int height;
};

class ChatApp
{
public:
    ChatApp();

    void Run();

    void AddView(IView& view);

private:
    void UpdateMainPanel();
    void PrepareNextFrame();
    void RenderFrame();
    void ApplyPostProcessing();

private:
    bool Init();
    void Update();
    void Uninit();

    bool WindowInit(std::string& outGlslVersion);
    void CreateUIContext();

    void SetupRendererBackend(const std::string& glslVersion);
    void SetupPostProcessing();

private:
    GLFWwindow* window{};

    std::string selectedChat;
    bool addNewChatPrompt{};

    struct PeerData
    {
        ImVector<char*> history;
        size_t unreadMessageCount;
    };
    // chat histories
    std::unordered_map<std::string, PeerData> historyMap;

    char InputBuf[256]{};
    ImVec4 clear_color;

    bool showDemoWindow{};
    const float resFactor = 0.5f;
    static constexpr int maxNameLength{32};
    PxlUI::ShaderProgram* ShaderProg{};
    GLuint FramebufferTexture{};
    GLuint Framebuffer{};

    char UsernameToConnectBuf[maxNameLength]{};

    // rendering
    struct CRTShaderData
    {
        float mBlur = 1.f;
        float mCurvature = 0.f;
        float mChroma = 0.15f;
        float mScanlineWidth = 2.0f;
        float mScanlineIntensity = 0.25f;
        float mVignette = 0.15f;
        float uCrtEnabled = true;
    } mCrtShaderData;

    DisplaySize frameDisplaySize;
    std::vector<std::reference_wrapper<IView>> views;
};
