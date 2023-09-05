#include "ChatApp.h"
#include "TranslationManager.h"
#include <emscripten/html5_webgl.h>


ChatApp::ChatApp(IWindow& windowManager_, IGUIManager& guiManager_)
    : windowManager(windowManager_), guiManager(guiManager_), frameDisplaySize()
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
    while (!windowManager.ShouldClose())
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
    if (!this->windowManager.Init("InformatiChats GLFW+ImGui+OpenGL3 demo", 1280, 720))
    {
        return false;
    }
#ifdef __EMSCRIPTEN__
    EmscriptenFullscreenStrategy strategy;
    strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
    strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
    strategy.canvasResizedCallback = emscripten_window_resized_callback;
    strategy.canvasResizedCallbackUserData = this->windowManager.GetNativeWindow();   // pointer to user data
    emscripten_enter_soft_fullscreen("canvas", &strategy);
#endif

    this->guiManager.Init();

    // Setup Platform/Renderer backends
    SetupRendererBackend();

    // setup post processing resources
    SetupPostProcessing();

    return true;
}

void ChatApp::Update()
{
    // Poll and handle events (inp      uts, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    this->windowManager.PollEvents();

    // Prepare the next opengl frame
    PrepareNextFrame();

    // Prepares the main pannel with all child panels for rendering
    UpdateMainPanel();

    // Render the frame
    RenderFrame();

    // draw the framebuffer texture onto the screen with the post processing shader
    ApplyPostProcessing();

    this->windowManager.SwapBuffers();
}

void ChatApp::Uninit()
{
    this->guiManager.Uninit();
    this->windowManager.Uninit();
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
    if (ImGui::Begin("Root panel", (bool*) 0,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
#ifndef __NDEBUG__
        // ImGui demo button and panel
        ImGui::BeginMainMenuBar();
        ImGui::Checkbox("Demo", &this->showDemoWindow);
        ImGui::EndMainMenuBar();

        // If the client is not connected to the server yet, ask the user to login.
        // Show the main application if connected.
        if (this->showDemoWindow)
            ImGui::ShowDemoWindow(&this->showDemoWindow);
#endif

        for (const auto& view: this->views)
        {
            view.get().Draw();
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
}

void ChatApp::PrepareNextFrame()
{
    this->frameDisplaySize.width = this->windowManager.GetWidth();
    this->frameDisplaySize.height = this->windowManager.GetHeight();

    int scaledDisplayWidth = this->frameDisplaySize.width * this->displayResolutionFactor;
    int scaledDisplayHeight = this->frameDisplaySize.height * this->displayResolutionFactor;

    // rendering new frame
    this->ShaderProg->setInt("uCrtEnabled", false);
    this->ShaderProg->setFloat2("iResolution", {this->frameDisplaySize.width, this->frameDisplaySize.height});
    //ShaderProg->setFloat("iTime", glfwGetTime()*100.0);

    glUseProgram(this->ShaderProg->getProgramId());

    glBindTexture(GL_TEXTURE_2D, this->FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scaledDisplayWidth, scaledDisplayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, this->Framebuffer);
    glClearColor(1.,1.,1.,1.);
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, scaledDisplayWidth, scaledDisplayHeight);

    this->guiManager.NewFrame();
}

void ChatApp::SetupRendererBackend()
{
    PxlUI::BatchRenderer::init();
}

void ChatApp::SetupPostProcessing()
{
    this->guiManager.SetDisplayResolutionFactor(this->displayResolutionFactor);

    this->ShaderProg = new PxlUI::ShaderProgram(VERT_SHADER, FRAG_SHADER);
    glUseProgram(this->ShaderProg->getProgramId());
    this->ShaderProg->bind();
    auto wLocation = glGetUniformLocation(this->ShaderProg->getProgramId(), "uTextures");
    int32_t wSamplers[32];
    for (int32_t i = 0; i < 32; i++)
    {
        wSamplers[i] = i;
    }
    glUniform1iv(wLocation, 32, wSamplers);

    this->ShaderProg->setFloat("uBlur", this->mCrtShaderData.mBlur);
    this->ShaderProg->setFloat("uCurvature", this->mCrtShaderData.mCurvature);
    this->ShaderProg->setFloat("uChroma", this->mCrtShaderData.mChroma);
    this->ShaderProg->setFloat("uScanlineWidth", this->mCrtShaderData.mScanlineWidth);
    this->ShaderProg->setFloat("uScanlineIntensity", this->mCrtShaderData.mScanlineIntensity);
    this->ShaderProg->setFloat("uVignette", this->mCrtShaderData.mVignette);
    this->ShaderProg->setInt("uCrtEnabled", false);
    this->ShaderProg->setFloat("iResFactor", this->displayResolutionFactor);

    glGenTextures(1, &this->FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, this->FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &this->Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->Framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FramebufferTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ChatApp::RenderFrame()
{
    guiManager.RenderFrame();

    // draw the framebuffer texture onto the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, this->frameDisplaySize.width, this->frameDisplaySize.height);
    glBindTexture(GL_TEXTURE_2D, FramebufferTexture);
}

void ChatApp::ApplyPostProcessing()
{
    this->ShaderProg->setInt("uCrtEnabled", true);

    PxlUI::BatchRenderer::beginBatch();

    PxlUI::BatchRenderer::drawScreenTex(this->FramebufferTexture);
    PxlUI::BatchRenderer::endBatch();
    PxlUI::BatchRenderer::flush();

}

void ChatApp::AddView(IView& view)
{
    this->views.emplace_back(view);
}
