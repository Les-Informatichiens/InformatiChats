//
// Created by Jonathan Richard on 2023-08-23.
//

#pragma once

#include <string>

enum class RendererAPI
{
    OpenGL
    //    WebGL,
    //    Vulkan
    /* ... */
};

class IWindow
{
public:
    virtual bool Init(const std::string& title, int width, int height) = 0;

    virtual bool ShouldClose() = 0;

    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;

    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;

    virtual void* GetNativeWindow() = 0;

    virtual void Uninit() = 0;
};
