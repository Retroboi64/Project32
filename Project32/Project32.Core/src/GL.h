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
