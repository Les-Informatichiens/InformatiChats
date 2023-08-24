//
// Created by Jonathan Richard on 2023-08-23.
//

#pragma once

#include "IWindow.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class GLFWWindowManager : public IWindow
{
public:
    explicit GLFWWindowManager(RendererAPI rendererApi_);

    bool Init(const std::string& title, int width, int height) override;

    bool ShouldClose() override;

    int GetWidth() override;
    int GetHeight() override;

    void PollEvents() override;
    void SwapBuffers() override;
    void* GetNativeWindow() override;
    void Uninit() override;

private:
    const RendererAPI rendererApi;

    GLFWwindow* window{nullptr};
};
