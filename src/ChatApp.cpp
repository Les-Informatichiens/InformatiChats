#include "ChatApp.h"
#include "TranslationManager.h"
#include <emscripten/html5_webgl.h>


static void glfw_error_callback(int error, const char* description);

void SetImGuiStyles();

ChatApp::ChatApp()
    : frameDisplaySize()
{
}

void ChatApp::Run()
{
    this->Init();

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    ImGui::GetIO().IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        this->Update();
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    this->Uninit();
}

EM_BOOL emscripten_window_resized_callback(int eventType, const void *reserved, void *userData){
    GLFWwindow* window = static_cast<GLFWwindow*>(userData);

    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);

    int w = (int)width, h = (int)height;

    // resize SDL window
    glfwSetWindowSize(window, w, h);
    return true;
}

bool ChatApp::Init()
{
    // create glfw window and get glsl shader version determined by opengl version
    std::string glslVersion;
    if (!WindowInit(glslVersion))
        return false;

#ifdef __EMSCRIPTEN__
    EmscriptenFullscreenStrategy strategy;
    strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
    strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
    strategy.canvasResizedCallback = emscripten_window_resized_callback;
    strategy.canvasResizedCallbackUserData = this->window;   // pointer to user data
    emscripten_enter_soft_fullscreen("canvas", &strategy);
#endif

    // create ImGui context and setup styles
    CreateUIContext();

    // Setup Platform/Renderer backends
    SetupRendererBackend(glslVersion);

    // setup post processing resources
    SetupPostProcessing();

    // message callback
    //    chatClient.SetOnMessageRecieved([&](const MessageReceivedEvent &e) {
    //        auto result = historyMap.insert({e.senderId, {}});
    //        result.first->second.history.push_back(Strdup(std::format("[{}] {}", e.senderId, e.content).c_str()));
    //
    //        if (e.senderId == selectedChat)
    //        {
    //            console.AddLog("[%s] %s", e.senderId.c_str(), e.content.c_str());
    //        }
    //        else
    //        {
    //            ++result.first->second.unreadMessageCount;
    //        }
    //    });

    return true;
}

void ChatApp::Update()
{
    // Poll and handle events (inp      uts, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Prepare the next opengl frame
    PrepareNextFrame();

    // Prepares the main pannel with all child panels for rendering
    UpdateMainPanel();

    // Render the frame
    RenderFrame();

    // draw the framebuffer texture onto the screen with the post processing shader
    ApplyPostProcessing();

    glfwSwapBuffers(window);
}

void ChatApp::Uninit()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void ChatApp::UpdateMainPanel()
{
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
    ImGui::Begin("Root panel", (bool*) 0,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoBringToFrontOnFocus);

    // ImGui demo button and panel
    ImGui::BeginMainMenuBar();
    ImGui::Checkbox("Demo", &showDemoWindow);
    ImGui::EndMainMenuBar();


    // If the client is not connected to the server yet, ask the user to login.
    // Show the main application if connected.
    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);

    for (const auto& view: views)
    {
        view.get().Draw();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void ChatApp::PrepareNextFrame()
{
    int scaledDisplayWidth, scaledDisplayHeight;
    glfwGetFramebufferSize(window, &(this->frameDisplaySize.width), &(this->frameDisplaySize.height));

    scaledDisplayWidth = this->frameDisplaySize.width * resFactor;
    scaledDisplayHeight = this->frameDisplaySize.height * resFactor;

    // rendering new frame
    ShaderProg->setInt("uCrtEnabled", false);
    ShaderProg->setFloat2("iResolution", {this->frameDisplaySize.width, this->frameDisplaySize.height});
    //        ShaderProg->setFloat("iTime", glfwGetTime()*100.0);

    glUseProgram(ShaderProg->getProgramId());

    glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scaledDisplayWidth, scaledDisplayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, scaledDisplayWidth, scaledDisplayHeight);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
}

bool ChatApp::WindowInit(std::string& outGlslVersion)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    outGlslVersion = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glslVersion = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);          // Required on Mac
#else
    // GL 3.0 + GLSL 130
    outGlslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return false;
    glfwMakeContextCurrent(window);
//    glfwSwapInterval(1);// Enable vsync

    return true;
}

void ChatApp::CreateUIContext()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_Portfolio_6x8.ttf", 8.0f);
    io.Fonts->AddFontFromFileTTF("res/fonts/Mx437_IBM_CGA.ttf", 8.0f);

    // Setup Dear ImGui style
    SetImGuiStyles();
}

void ChatApp::SetupRendererBackend(const std::string& glslVersion)
{
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
    PxlUI::BatchRenderer::init();
}

void ChatApp::SetupPostProcessing()
{
    ImGui_ImplGlfw_SetResFactor(resFactor);

    ShaderProg = new PxlUI::ShaderProgram(VERT_SHADER, FRAG_SHADER);
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
    ShaderProg->setFloat("iResFactor", resFactor);

    glGenTextures(1, &FramebufferTexture);
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
}

void ChatApp::RenderFrame()
{
    ImGui::Render();

    glClearColor(0.2,0.0,0.1,0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // draw the framebuffer texture onto the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, this->frameDisplaySize.width, this->frameDisplaySize.height);

    glBindTexture(GL_TEXTURE_2D, FramebufferTexture);

}

void ChatApp::ApplyPostProcessing()
{
    ShaderProg->setInt("uCrtEnabled", true);

    PxlUI::BatchRenderer::beginBatch();
    PxlUI::BatchRenderer::drawScreenTex(FramebufferTexture);
    PxlUI::BatchRenderer::endBatch();
    PxlUI::BatchRenderer::flush();

//    PxlUI::BatchRenderer::beginBatch();
//    PxlUI::BatchRenderer::drawTriangle({-0.5, -0.5, 0}, {0.0, 0.5, 0}, {0.5, -0.5, 0}, {1,1,1,1});
//    PxlUI::BatchRenderer::beginBatch();
//    PxlUI::BatchRenderer::flush();
}

void ChatApp::AddView(IView& view)
{
    views.emplace_back(view);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void SetImGuiStyles()
{
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
