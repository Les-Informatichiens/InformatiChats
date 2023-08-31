//
// Created by Jonathan Richard on 2023-08-09.
//

#pragma once

#include <View/Views/IView.h>
#include <View/pxlui/BatchRenderer.h>
#include <View/pxlui/GLShaderUtil.h>
#include <View/pxlui/ShaderProgram.h>

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

#include <View/Backend/GLFWWindowManager.h>
#include <View/GUI/ImGuiManager.hpp>
#include <View/Shaders.h>
#include <vector>


struct DisplaySize
{
    int width;
    int height;
};

class ChatApp
{
public:
    ChatApp(IWindow& windowManager_, IGUIManager& guiManager_);

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

    void SetupRendererBackend();
    void SetupPostProcessing();

private:
    IWindow& windowManager;
    IGUIManager& guiManager;

    bool showDemoWindow{};

    const float displayResolutionFactor = 0.5f;
    PxlUI::ShaderProgram* ShaderProg{};
    GLuint FramebufferTexture{};
    GLuint Framebuffer{};

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
