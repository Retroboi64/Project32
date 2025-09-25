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
#include <functional>
#include <string>
#include <vector>
#include <memory>

struct ImGuiContext;

enum class CursorMode {
    Normal,
    Hidden,
    Disabled
};

class Window {
private:
    GLFWwindow* _window = nullptr;
    int _width;
    int _height;

    std::string _title;
    std::string _iconPath;
    bool _vsync = true;
    bool _isOpen = false;
    bool _isFullscreen = false;
    static int _nextID;
    int _ID;

    glm::ivec2 _windowedPos{ 100, 100 };
    glm::ivec2 _windowedSize{ 800, 600 };

    CursorMode _cursorMode = CursorMode::Disabled;

    std::function<void(int, int)> _resizeCallback;

    ImGuiContext* _imguiContext = nullptr;
    bool _imguiInitialized = false;

public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void Init();
    void InitGLAD();
    void InitImGui();
    void Shutdown();

    void PollEvents();
    void SwapBuffers();
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void MakeContextCurrent();

    bool IsOpen() const;
    void SetShouldClose(bool value);

    void BeginImGuiFrame();
    void EndImGuiFrame();

    void GetSize(int& width, int& height) const;
    glm::ivec2 GetSize() const;
    void SetSize(int width, int height);

    void GetPosition(int& x, int& y) const;
    glm::ivec2 GetPosition() const;
    void SetPosition(int x, int y);

    void SetTitle(const std::string& title);
    void SetIcon(const std::string& iconPath);

    float GetAspectRatio() const;

    void SetVSync(bool enabled);
    void ToggleFullscreen();

    void SetCursorMode(CursorMode mode);
    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    glm::vec2 GetMousePosition() const;
    void SetMousePosition(float x, float y);

    void SetResizeCallback(std::function<void(int, int)> callback);
    void OnFramebufferResize(int width, int height);

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* GetGLFWwindow() const { return _window; }
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
    int GetID() const { return _ID; }
    const std::string& GetTitle() const { return _title; }
    bool IsVSync() const { return _vsync; }
    bool IsFullscreen() const { return _isFullscreen; }
    bool WindowIsOpen() const { return _window != nullptr && _isOpen; }
    bool IsWindowValid() const { return _window != nullptr; }
    CursorMode GetCursorMode() const { return _cursorMode; }

    ImGuiContext* GetImGuiContext() const { return _imguiContext; }
    bool IsImGuiInitialized() const { return _imguiInitialized; }

    static constexpr int KEY_ESCAPE = GLFW_KEY_ESCAPE;
    static constexpr int KEY_W = GLFW_KEY_W;
    static constexpr int KEY_A = GLFW_KEY_A;
    static constexpr int KEY_S = GLFW_KEY_S;
    static constexpr int KEY_D = GLFW_KEY_D;
    static constexpr int KEY_SPACE = GLFW_KEY_SPACE;
    static constexpr int KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
    static constexpr int KEY_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
    static constexpr int KEY_F11 = GLFW_KEY_F11;

    static constexpr int MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT;
    static constexpr int MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT;
    static constexpr int MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE;
};

class WindowManager {
private:
    std::vector<std::unique_ptr<Window>> _windows;
    int currentWindow = -1;

public:
    int Count();
    int GetCurrentWindowID() const { return currentWindow; }
    int AddWindow(int width, int height, const std::string& name);
    int RemoveWindow(int index);

    void SetCurrentWindow(int index);
    std::string GetWindowTitle(int index);

    Window* GetWindowByID(int index);
    Window* GetWindowByTitle(const std::string& title);
    Window* GetCurrentWindow();
};