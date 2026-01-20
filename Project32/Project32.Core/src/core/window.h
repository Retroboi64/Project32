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

#ifndef WINDOW_H
#define WINDOW_H

#include "../renderer/renderer.h"
#include "ui.h"
#include "input.h"

struct ImGuiContext;
struct GLFWwindow;
class Renderer;
class UIX;
class Input;

enum class CursorMode {
    Normal,
    Hidden,
    Disabled
};

class Window {
private:
    GLFWwindow* _window = nullptr;

    std::unique_ptr<UIX> _ui;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<Input> _input;

    int _width;
    int _height;

    std::string _title;
    std::string _iconPath;
    bool _vsync = true;
    bool _isOpen = false;
    bool _isFullscreen = false;
    static int _nextID;
    int _ID;

    Engine* _engine;

    glm::ivec2 _windowedPos{ 100, 100 };
    glm::ivec2 _windowedSize{ 800, 600 };

    CursorMode _cursorMode = CursorMode::Disabled;

    std::function<void(int, int)> _resizeCallback;

    ImGuiContext* _imguiContext = nullptr;
    bool _imguiInitialized = false;

public:
    Window(int width, int height, const std::string& title, GLFWwindow* shareContext, Engine* engine = nullptr);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Engine* GetEngine() const { return _engine; }
    void SetEngine(Engine* engine) { _engine = engine; }

    void Init();
    void InitGLAD();
    void Shutdown();

    void Render(); 
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
    int SetSize(int width, int height);

    void GetPosition(int& x, int& y) const;
    glm::ivec2 GetPosition() const;
    void SetPosition(int x, int y);

	void MouseLocked(bool locked);

    void SetTitle(const std::string& title);
    void SetIcon(const std::string& iconPath);

    float GetAspectRatio() const;

    void SetVSync(bool enabled);
    void ToggleFullscreen();

    void SetMSAASamples(UINT count);

    void SetCursorMode(CursorMode mode);
    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    glm::vec2 GetMousePosition() const;
    void SetMousePosition(float x, float y);

    void SetResizeCallback(std::function<void(int, int)> callback);
    void OnFramebufferResize(int width, int height);

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    Renderer* GetRenderer() const;
    UIX* GetUI() const;
    Input* GetInput() const;

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
};

class WindowManager {
private:
	Engine* _engine = nullptr;
    std::vector<std::unique_ptr<Window>> _windows;
    int currentWindow = -1;
    mutable std::mutex _windowsMutex;  

public:
	WindowManager(Engine* engine) : _engine(engine) {}
	~WindowManager() = default;

    int Count();
    int GetCurrentWindowID() const { return currentWindow; }

    Window* GetWindowAt(int index);
    Window* GetWindowByID(int windowID);
    Window* GetWindowByTitle(const std::string& title);
    Window* GetCurrentWindow();

    int AddWindow(int width, int height, const std::string& name);
    int RemoveWindow(int windowID);
    void RemoveAllWindows();
    void Cleanup();

    void SetCurrentWindow(int windowID);
    std::string GetWindowTitle(int windowID);
};

#endif // WINDOW_H