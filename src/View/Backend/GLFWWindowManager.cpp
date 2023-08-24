//
// Created by Jonathan Richard on 2023-08-23.
//

#include "GLFWWindowManager.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GLFWWindowManager::GLFWWindowManager(RendererAPI rendererApi_)
    : rendererApi(rendererApi_)
{
}

bool GLFWWindowManager::Init(const std::string& title, int width, int height)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;

    // set gl versions
    switch (rendererApi)
    {
        case RendererAPI::OpenGL:
        {
            break;
        }
    }

    // Create window with graphics context
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
        return false;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);// Enable vsync by default

    return true;
}

bool GLFWWindowManager::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

int GLFWWindowManager::GetWidth()
{
    int w;
    glfwGetFramebufferSize(window, &w, nullptr);
    return w;
}

int GLFWWindowManager::GetHeight()
{
    int h;
    glfwGetFramebufferSize(window, nullptr, &h);
    return h;
}

void GLFWWindowManager::PollEvents()
{
    glfwPollEvents();
}

void GLFWWindowManager::SwapBuffers()
{
    glfwSwapBuffers(window);
}

void* GLFWWindowManager::GetNativeWindow()
{
    return window;
}

void GLFWWindowManager::Uninit()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}