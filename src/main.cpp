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

// Main code
int main(int, char**)
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
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_Portfolio_6x8.ttf", 8.0f);
    io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_IBM_CGA.ttf", 8.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Pixel_Init(glsl_version);
    PxlUI::BatchRenderer::init();

    // Our state
    constexpr const int maxNameLength = 32;

    ConnectionConfig config = { stunServer, stunServerPort, signalingServer, signalingServerPort };

    Chat chatClient(config);

    bool showDemoWindow = false;
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

    bool addNewChatPrompt = false;

    std::string selectedChat;

    char InputBuf[256];
    memset(InputBuf, 0, sizeof(InputBuf));

    // chat message space
    ExampleAppConsole console;
    bool consoleOpen;

    struct PeerData
    {
        ImVector<char*> history;
        size_t unreadMessageCount;
    };

    // chat histories
    std::unordered_map<std::string, PeerData> historyMap;

    // message callback
    chatClient.SetOnMessageRecieved([&console, &selectedChat, &historyMap](const MessageReceivedEvent& e) {
        auto result = historyMap.insert({ e.senderId, {} });
        result.first->second.history.push_back(Strdup(std::format("[{}] {}", e.senderId, e.content).c_str()));

        if (e.senderId == selectedChat)
        {
            console.AddLog("[%s] %s", e.senderId.c_str(), e.content.c_str());
        }
        else
        {
            ++result.first->second.unreadMessageCount;
        }
    });

    // rendering
    GLuint ShaderProgram;
    GLuint FramebufferTexture;
    GLuint Framebuffer;

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

    const float resFactor = 0.5f;
    ImGui_ImplGlfw_SetResFactor(resFactor);

    PxlUI::ShaderProgram* ShaderProg = new PxlUI::ShaderProgram(VERT_SHADER, FRAG_SHADER);
    glUseProgram(ShaderProg->getProgramId());
    ShaderProg->bind();
    auto wLocation = glGetUniformLocation(ShaderProg->getProgramId(), "uTextures");
    int32_t wSamplers[32];
    for (int32_t i = 0; i < 32; i++)
    {
        wSamplers[i] = i;
    }
    glUniform1iv(wLocation, 32, wSamplers);

    ShaderProg->setFloat("uBlur", mCrtShaderData.mBlur);
    ShaderProg->setFloat("uCurvature", mCrtShaderData.mCurvature);
    ShaderProg->setFloat("uChroma", mCrtShaderData.mChroma);
    ShaderProg->setFloat("uScanlineWidth", mCrtShaderData.mScanlineWidth);
    ShaderProg->setFloat("uScanlineIntensity", mCrtShaderData.mScanlineIntensity);
    ShaderProg->setFloat("uVignette", mCrtShaderData.mVignette);
    ShaderProg->setInt("uCrtEnabled", false);

    glCreateTextures(GL_TEXTURE_2D, 1, &FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FramebufferTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // =========

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        int display_w, display_h;
        int scaled_display_w, scaled_display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        scaled_display_w = display_w * resFactor;
        scaled_display_h = display_h * resFactor;

        // rendering new frame
        ShaderProg->setInt("uCrtEnabled", false);
        ShaderProg->setFloat2("iResolution", { scaled_display_w, scaled_display_h });
//        ShaderProg->setFloat("iTime", glfwGetTime()*100.0);

        glUseProgram(ShaderProg->getProgramId());

        glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scaled_display_w, scaled_display_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
        glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, scaled_display_w, scaled_display_h);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_Pixel_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* viewport = nullptr;
        viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
#else
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("main_shit", (bool*)0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::BeginMainMenuBar();
        ImGui::Checkbox("Demo", &showDemoWindow);
        ImGui::EndMainMenuBar();

        if (chatClient.IsConnected())
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("Npcs", ImVec2(200, -50), true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar);
            if (ImGui::Button("Add new user"))
            {
                addNewChatPrompt = true;
            }
            if (addNewChatPrompt)
            {
                ImGui::OpenPopup("New chat");
                if (ImGui::BeginPopupModal("New chat", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Enter a username");
                    ImGui::Separator();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                    char buf[maxNameLength];
                    memset(buf, 0, maxNameLength*sizeof(char));
                    if (ImGui::InputTextWithHint("##username", "otismusia...", buf, maxNameLength, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        chatClient.AttemptToConnectToPeer(buf);
                        addNewChatPrompt = false;
                    }
                    ImGui::PopStyleVar();

                    ImGui::EndPopup();
                }
            }
            ImGui::Separator();

            // draw chat names
            for (const auto& peerConnection : chatClient.GetPeerConnections())
            {
                const std::string& peerId = peerConnection.first;
                bool isSelected = selectedChat == peerId;

                rtc::PeerConnection::State state = peerConnection.second->state();

                std::string displayText = peerId;
                ImVec4 color;
                bool hasColor = false;
                switch (state) {
                    case rtc::PeerConnection::State::New:
                        break;
                    case rtc::PeerConnection::State::Connecting: {
                        hasColor = true;
                        color = ImVec4(1.0f, 0.9f, 0.2f, 1.0f);
                        displayText += " [Connecting...]";
                        break;
                    }
                    case rtc::PeerConnection::State::Connected:
                        break;
                    case rtc::PeerConnection::State::Disconnected:
                    {
                        hasColor = true;
                        color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        displayText += " [Disconnected]";
                        break;
                    }
                    case rtc::PeerConnection::State::Failed: {
                        hasColor = true;
                        color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        displayText += " [Connection failed]";
                        break;
                    }
                    case rtc::PeerConnection::State::Closed: {
                        hasColor = true;
                        color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        displayText += " [Connection closed]";
                        break;
                    }
                    default: {
                        hasColor = false;
                        break;
                    }
                }

                auto peerDataIt = historyMap.find(peerId);
                PeerData* pPeerData = nullptr;
                if (peerDataIt != historyMap.end())
                {
                    pPeerData = &peerDataIt->second;
                }

                if (pPeerData != nullptr)
                {
                    if (isSelected)
                    {
                        pPeerData->unreadMessageCount = 0;
                    }
                    if (pPeerData->unreadMessageCount > 0)
                    {
                        displayText += std::format(" [{} unread message{}]", pPeerData->unreadMessageCount, pPeerData->unreadMessageCount == 1 ? "" : "s");
                    }
                }

                if (hasColor) ImGui::PushStyleColor(ImGuiCol_Text, color);
                if (ImGui::Selectable(displayText.c_str(), isSelected))
                {
                    if (pPeerData != nullptr)
                        console.SetLogHistory(pPeerData->history);
                    selectedChat = peerId;
                }
                if (hasColor) ImGui::PopStyleColor();

            }
            ImGui::EndChild();

            ImGui::BeginChild("userid", ImVec2(200, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Text("%s", chatClient.GetUsername().c_str());

            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            auto time = glfwGetTime();
            for (double i = 0; i < 20; ++i) {
                ImVec2 p1;
                p1.x = ImGui::GetCursorScreenPos().x + i*2,
                p1.y = ImGui::GetCursorScreenPos().y + 5;
                ImVec2 p2;
                p2.x = ImGui::GetCursorScreenPos().x + i*2,
                p2.y = ImGui::GetCursorScreenPos().y + 5*glm::sin(time+i/3.0) + 5;
                draw_list->AddLine(p1, p2, 0xFFFFFFFF, 1);

            }

            ImGui::EndChild();
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::BeginChild("Chat", ImVec2(ImGui::GetContentRegionAvail().x, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
            console.Draw("Demo chat", &consoleOpen);

            // Command-line
            bool reclaim_focus = false;
            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
            if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &ExampleAppConsole::TextEditCallbackStub, (void*)&console))
            {
                char* s = InputBuf;
                Strtrim(s);
                if (s[0])
                {
                    // duplicate code, pls clean up
                    auto result = historyMap.insert({ selectedChat, {} });
                    result.first->second.history.push_back(Strdup(std::format("[{}] {}", chatClient.GetUsername(), s).c_str()));

                    console.AddLog("[%s] %s", chatClient.GetUsername().c_str(), s);
                    chatClient.SendMessageToPeer(selectedChat, s);
                }
                strcpy(s, "");
                reclaim_focus = true;
            }

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

            ImGui::EndChild();
        }
        else
        {
            ImGui::OpenPopup("Login");
            if (ImGui::BeginPopupModal("Login", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Enter your username");
                ImGui::Separator();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                char buf[maxNameLength];
                memset(buf, 0, maxNameLength*sizeof(char));
                if (ImGui::InputTextWithHint("##username", "otisma...", buf, maxNameLength, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    chatClient.AttemptConnectionWithUsername(buf);
                }
                ImGui::PopStyleVar();

                ImGui::EndPopup();
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();

        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);

        // Rendering
        ImGui::Render();

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_Pixel_RenderDrawData(ImGui::GetDrawData());

        // draw the framebuffer texture onto the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, display_w, display_h);
        glBindTexture(GL_TEXTURE_2D, FramebufferTexture);

        // draw the framebuffer texture onto the screen with the post processing shader
        ShaderProg->setInt("uCrtEnabled", true);

        PxlUI::BatchRenderer::beginBatch();

        PxlUI::BatchRenderer::drawScreenTex(FramebufferTexture);
        PxlUI::BatchRenderer::endBatch();
        PxlUI::BatchRenderer::flush();

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Pixel_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}