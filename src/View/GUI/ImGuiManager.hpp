//
// Created by Jonathan Richard on 2023-08-23.
//

#pragma once

#include "IGUIManager.h"
#include "View/Backend/GLFWWindowManager.h"
#include "View/imgui_impl_glfw_pixel.h"
#include "View/imgui_impl_opengl3_pixel.h"
#include "backends/imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)

#include <GLES2/gl2.h>
#else

#include <GL/glew.h>
#endif

namespace {

    void CreateUIContext()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    }

    void SetImGuiStyles()
    {
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_Portfolio_6x8.ttf", 8.0f);
        io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_IBM_CGA.ttf", 8.0f);

        ImGuiStyle& style = ImGui::GetStyle();

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.61f, 0.71f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.08f, 0.15f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.08f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.61f, 0.71f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.75f, 0.80f, 0.86f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.64f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.64f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.35f, 0.41f, 0.53f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.00f, 0.27f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.15f, 0.17f, 0.27f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.24f, 0.15f, 0.19f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.89f, 0.23f, 0.27f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.24f, 0.15f, 0.19f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.45f, 0.24f, 0.22f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.00f, 0.27f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.09f, 0.08f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.61f, 0.71f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.97f, 0.46f, 0.13f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.91f, 0.38f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.68f, 0.20f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.23f, 0.27f, 0.40f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.35f, 0.41f, 0.53f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.91f, 0.38f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.75f, 0.80f, 0.86f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.75f, 0.80f, 0.86f, 0.35f);

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
}

template<typename TWindowManager, RendererAPI RendererAPI>
class ImGuiManager : public IGUIManager
{
    static_assert(std::is_base_of<IWindow, TWindowManager>::value,
                  "WindowManagerType must inherit from WindowManager");
public:
    explicit ImGuiManager(TWindowManager& windowManager_)
        : windowManager(windowManager_)
    {
    }

    bool Init() override;

    void SetDisplayResolutionFactor(float displayResolutionFactor) override;

    void NewFrame() override;
    void RenderFrame() override;
    void Uninit() override;

private:
    TWindowManager& windowManager;
};

template<>
class ImGuiManager<GLFWWindowManager, RendererAPI::OpenGL> : public IGUIManager
{
public:
    explicit ImGuiManager(GLFWWindowManager& windowManager_)
        : windowManager(windowManager_)
    {
    }

    bool Init() override
    {
        CreateUIContext();
        SetImGuiStyles();

        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(windowManager.GetNativeWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 300 es");
        return false;
    }

    void SetDisplayResolutionFactor(float displayResolutionFactor) override
    {
        ImGui_ImplGlfw_SetResFactor(displayResolutionFactor);
    }

    void NewFrame() override
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void RenderFrame() override
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Uninit() override
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

private:
    GLFWWindowManager& windowManager;
};

