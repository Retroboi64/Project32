/*
 * This file is part of Project32 - A compact yet powerful and flexible C++ Game Engine
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT with Attribution Requirements
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/Project32
 *
 * This header must not be removed from any source file.
 */

#pragma once

#include "common.h"

namespace GL {
    extern GLFWwindow* _window;
    extern int _windowWidth;
    extern int _windowHeight;
    extern bool _vsync;

    void SetVSync(bool enabled);
    void ToggleFullscreen();
    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void Init(int width, int height, std::string title);
    GLFWwindow* GetWindowPointer();
    bool WindowIsOpen();
    void SetWindowShouldClose(bool value);
    void SwapBuffersPollEvents();
    void Cleanup();
    glm::ivec2 GetWindowSize();
	void BeginImGuiFrame();
	void EndImGuiFrame();
}
