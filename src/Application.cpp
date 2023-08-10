#include "Application.h"

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

const std::string stunServer = "stun.l.google.com";
const std::string stunServerPort = "19302";
const std::string signalingServer = "51.79.86.30";
const std::string signalingServerPort = "51337";

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void SetImGuiStyles()
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.55f, 0.61f, 0.71f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.09f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.09f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.55f, 0.61f, 0.71f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.75f, 0.80f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.64f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.64f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.35f, 0.41f, 0.53f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(1.00f, 0.00f, 0.27f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.24f, 0.15f, 0.19f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.24f, 0.15f, 0.19f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.45f, 0.24f, 0.22f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(1.00f, 0.00f, 0.27f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.09f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.55f, 0.61f, 0.71f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.97f, 0.46f, 0.13f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.91f, 0.38f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.68f, 0.20f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 0.91f, 0.38f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.75f, 0.80f, 0.86f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.75f, 0.80f, 0.86f, 0.35f);

    // Make everything more square
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ChildBorderSize = 2.0f;
    style.TabBorderSize = 0.0f;

    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.ScrollbarSize = 12.0f;
}

bool Application::Init()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    this->window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (this->window == nullptr)
        return 1;
    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_Portfolio_6x8.ttf", 8.0f);
    io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_IBM_CGA.ttf", 8.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Pixel_Init(glsl_version);
    PxlUI::BatchRenderer::init();

    // message callback
    this->chatClient.SetOnMessageRecieved([this](const MessageReceivedEvent& e) {
        auto result = this->historyMap.insert({ e.senderId, {} });
        result.first->second.history.push_back(Strdup(std::format("[{}] {}", e.senderId, e.content).c_str()));

        if (e.senderId == this->selectedChat)
        {
            this->console.AddLog("[%s] %s", e.senderId.c_str(), e.content.c_str());
        }
        else
        {
            ++result.first->second.unreadMessageCount;
        }
    });

    /* missing stuff here */

    return 0;
}

void Application::Update()
{

}

void Application::Uninit()
{

}
