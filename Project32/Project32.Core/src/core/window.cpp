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

#include "../common.h"
#include "window.h"
#include "../renderer/renderer.h"

int Window::_nextID = 0;

static void StaticFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (window) {
        window->OnFramebufferResize(width, height);
    }
}

Window::Window(int width, int height, const std::string& title, GLFWwindow* shareContext)
    : _width(width), _height(height), _title(title), _ID(_nextID++)
{
    spdlog::info("[Window {}] Constructor called: {}x{} '{}'", _ID, width, height, title);

    static bool glfwInitialized = false;
    if (!glfwInitialized) {
        if (!glfwInit()) {
            spdlog::error("[Window {}] Failed to initialize GLFW", _ID);
            throw std::runtime_error("Failed to initialize GLFW");
        }
        glfwInitialized = true;
        spdlog::info("[Window {}] GLFW initialized successfully", _ID);
    }

    if (GraphicsBackend::GetCurrentType() == BackendType::OPENGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }
    else if (GraphicsBackend::GetCurrentType() == BackendType::UNDEFINED) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, shareContext);
    if (!_window) {
        spdlog::error("[Window {}] Failed to create GLFW window", _ID);
        throw std::runtime_error("Failed to create GLFW window");
    }

    spdlog::info("[Window {}] GLFW window created at: {}", _ID, static_cast<void*>(_window));

    glfwSetWindowUserPointer(_window, this);
    glfwMakeContextCurrent(_window);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(_window, StaticFramebufferSizeCallback);

    static bool gladInitialized = false;
    if (!gladInitialized) {
        InitGLAD();
        gladInitialized = true;
    }

    if (GraphicsBackend::GetCurrentType() == BackendType::UNDEFINED) {
        GraphicsBackend::Initialize(BackendType::OPENGL);
        GraphicsTypes::Initialize();
        GraphicsBackend::Get()->SetViewport(0, 0, width, height);
        spdlog::info("[Renderer] Initialized graphics backend");
    }

    SetVSync(_vsync);
    _isOpen = true;

    _input = std::make_unique<Input>(this);
    _input->Init();
    spdlog::info("[Window {}] Input initialized successfully", _ID);

    _ui = std::make_unique<UIX>(_window);
    spdlog::info("[Window {}] UI initialized successfully", _ID);

    _renderer = std::make_unique<Renderer>(this);
    _renderer->Init();
    spdlog::info("[Window {}] Renderer initialized successfully", _ID);

    spdlog::info("[Window {}] Created successfully", _ID);
    spdlog::info("[Window {}] OpenGL Version: {}", _ID, reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("[Window {}] GPU: {}", _ID, reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}

Window::~Window() {
    spdlog::info("[Window {}] Destructor called", _ID);

    if (_input) {
        _input.reset();
    }

    if (_renderer) {
        _renderer->Cleanup();
        _renderer.reset();
    }

    if (_ui && _window) {
        _ui->Cleanup(_window);
        _ui.reset();
    }

    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }

    spdlog::info("[Window {}] Destructor completed", _ID);
}

void Window::Init() {
    spdlog::info("[Window {}] Init() called (initialization already done in constructor)", _ID);
}

void Window::InitGLAD() {
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD for Window " + std::to_string(_ID));
    }
    spdlog::info("[Window {}] GLAD initialized successfully", _ID);
}

void Window::Shutdown() {
    if (!_window) return;

    spdlog::info("[Window {}] Shutdown called", _ID);

    _isOpen = false;

    if (_input) {
        _input.reset();
    }

    if (_renderer) {
        MakeContextCurrent();
        _renderer->Cleanup();
        _renderer.reset();
    }

    if (_ui) {
        MakeContextCurrent();
        _ui->Cleanup(_window);
        _ui.reset();
    }

    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }

    spdlog::info("[Window {}] Shutdown complete", _ID);
}

void Window::Render() {
    if (!_window || !_renderer || !_renderer->IsReady()) {
        return;
    }

    MakeContextCurrent();
}

void Window::PollEvents() {
    if (_window) {
        _isOpen = !glfwWindowShouldClose(_window);
    }

    if (_input) {
        _input->Update();
    }
}

void Window::SetMSAASamples(UINT count) {
    if (GraphicsBackend::GetCurrentType() == BackendType::OPENGL) {
        glfwWindowHint(GLFW_SAMPLES, count);
    }
}

void Window::SwapBuffers() {
    if (_window) {
        glfwSwapBuffers(_window);
    }
}

bool Window::IsOpen() const {
    return _window && _isOpen && !glfwWindowShouldClose(_window);
}

void Window::SetShouldClose(bool value) {
    if (_window) {
        glfwSetWindowShouldClose(_window, value);
        if (value) {
            _isOpen = false;
        }
    }
}

void Window::GetSize(int& width, int& height) const {
    if (_window) {
        glfwGetWindowSize(_window, &width, &height);
    }
    else {
        width = _width;
        height = _height;
    }
}

glm::ivec2 Window::GetSize() const {
    int width, height;
    GetSize(width, height);
    return glm::ivec2(width, height);
}

int Window::SetSize(int width, int height) {
    if (_window) {
        glfwSetWindowSize(_window, width, height);
    }
    _width = width;
    _height = height;
    return 0;
}

void Window::GetPosition(int& x, int& y) const {
    if (_window) {
        glfwGetWindowPos(_window, &x, &y);
    }
    else {
        x = y = 0;
    }
}

glm::ivec2 Window::GetPosition() const {
    int x, y;
    GetPosition(x, y);
    return glm::ivec2(x, y);
}

void Window::SetPosition(int x, int y) {
    if (_window) {
        glfwSetWindowPos(_window, x, y);
    }
}

void Window::SetVSync(bool enabled) {
    _vsync = enabled;
    if (_window) {
        glfwMakeContextCurrent(_window);
        glfwSwapInterval(enabled ? 1 : 0);
    }
}

void Window::ToggleFullscreen() {
    if (!_window) return;

    if (_isFullscreen) {
        glfwSetWindowMonitor(_window, nullptr, _windowedPos.x, _windowedPos.y,
            _windowedSize.x, _windowedSize.y, 0);
    }
    else {
        glfwGetWindowPos(_window, &_windowedPos.x, &_windowedPos.y);
        glfwGetWindowSize(_window, &_windowedSize.x, &_windowedSize.y);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    _isFullscreen = !_isFullscreen;
}

void Window::SetTitle(const std::string& title) {
    _title = title;
    if (_window) {
        glfwSetWindowTitle(_window, title.c_str());
    }
}

void Window::SetIcon(const std::string& iconPath) {
    _iconPath = iconPath;
}

void Window::MakeContextCurrent() {
    if (_window) {
        glfwMakeContextCurrent(_window);

        if (_ui && _ui->IsInitialized()) {
            ImGui::SetCurrentContext(_ui->GetContext());
        }
    }
}

void Window::SetCursorMode(CursorMode mode) {
    if (!_window) return;

    int glfwMode;
    switch (mode) {
    case CursorMode::Normal:
        glfwMode = GLFW_CURSOR_NORMAL;
        break;
    case CursorMode::Hidden:
        glfwMode = GLFW_CURSOR_HIDDEN;
        break;
    case CursorMode::Disabled:
        glfwMode = GLFW_CURSOR_DISABLED;
        break;
    default:
        glfwMode = GLFW_CURSOR_NORMAL;
    }
    glfwSetInputMode(_window, GLFW_CURSOR, glfwMode);
    _cursorMode = mode;
}

bool Window::IsKeyPressed(int key) const {
    return _window && glfwGetKey(_window, key) == GLFW_PRESS;
}

bool Window::IsMouseButtonPressed(int button) const {
    return _window && glfwGetMouseButton(_window, button) == GLFW_PRESS;
}

glm::vec2 Window::GetMousePosition() const {
    if (!_window) return glm::vec2(0.0f);

    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Window::SetMousePosition(float x, float y) {
    if (_window) {
        glfwSetCursorPos(_window, x, y);
    }
}

float Window::GetAspectRatio() const {
    return static_cast<float>(_width) / static_cast<float>(_height);
}

void Window::Clear(float r, float g, float b, float a) {
    if (_window) {
        glfwMakeContextCurrent(_window);
        glm::vec4 color(r, g, b, a);
        GraphicsBackend::Get()->Clear(color);
    }
}

void Window::OnFramebufferResize(int width, int height) {
    _width = width;
    _height = height;

    if (_window) {
        glfwMakeContextCurrent(_window);
        GraphicsBackend::Get()->SetViewport(0, 0, width, height);
    }

    if (_resizeCallback) {
        _resizeCallback(width, height);
    }
}

void Window::SetResizeCallback(std::function<void(int, int)> callback) {
    _resizeCallback = callback;
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    GraphicsBackend::Get()->SetViewport(0, 0, width, height);
}

void Window::BeginImGuiFrame() {
    if (!_window || !_ui || !_ui->IsInitialized()) {
        spdlog::warn("[Window {}] BeginImGuiFrame called but ImGui not properly initialized!", _ID);
        return;
    }

    glfwMakeContextCurrent(_window);
    _ui->BeginImgui();
}

void Window::EndImGuiFrame() {
    if (!_window || !_ui || !_ui->IsInitialized()) {
        spdlog::warn("[Window {}] EndImGuiFrame called but ImGui not properly initialized!", _ID);
        return;
    }

    glfwMakeContextCurrent(_window);
    _ui->EndImgui();
}

Renderer* Window::GetRenderer() const {
    return _renderer.get();
}

UIX* Window::GetUI() const {
    return _ui.get();
}

Input* Window::GetInput() const {
    return _input.get();
}

int WindowManager::Count() {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    return static_cast<int>(_windows.size());
}

Window* WindowManager::GetWindowAt(int index) {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    if (index >= 0 && index < static_cast<int>(_windows.size())) {
        return _windows[index].get();
    }
    return nullptr;
}

Window* WindowManager::GetWindowByID(int windowID) {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    for (const auto& window : _windows) {
        if (window && window->GetID() == windowID) {
            return window.get();
        }
    }
    return nullptr;
}

int WindowManager::AddWindow(int width, int height, const std::string& name) {
    try {
        GLFWwindow* shareContext = nullptr;

        {
            std::lock_guard<std::mutex> lock(_windowsMutex);
            if (!_windows.empty()) {
                shareContext = _windows.front()->GetGLFWwindow();
            }
        }

        auto window = std::make_unique<Window>(width, height, name, shareContext);
        int windowID = window->GetID();

        {
            std::lock_guard<std::mutex> lock(_windowsMutex);
            _windows.push_back(std::move(window));
            spdlog::info("[WindowManager] Added window with ID: {} at index: {}", windowID, _windows.size() - 1);
        }

        return windowID;
    }
    catch (const std::exception& e) {
        spdlog::error("[WindowManager] Failed to add window: {}", e.what());
        return -1;
    }
}

int WindowManager::RemoveWindow(int windowID) {
    std::lock_guard<std::mutex> lock(_windowsMutex);

    for (auto it = _windows.begin(); it != _windows.end(); ++it) {
        if ((*it) && (*it)->GetID() == windowID) {
            spdlog::info("[WindowManager] Removing window with ID: {}", windowID);

            (*it)->Shutdown();
            _windows.erase(it);

            if (currentWindow == windowID) {
                currentWindow = -1;
            }
            return currentWindow;
        }
    }
    spdlog::error("[WindowManager] Window with ID {} not found for removal", windowID);
    return -1;
}

void WindowManager::RemoveAllWindows() {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    spdlog::info("[WindowManager] Removing all windows ({})", _windows.size());

    for (auto& window : _windows) {
        if (window) {
            window->Shutdown();
        }
    }

    _windows.clear();
    currentWindow = -1;
}

void WindowManager::Cleanup() {
    RemoveAllWindows();
}

void WindowManager::SetCurrentWindow(int windowID) {
    std::lock_guard<std::mutex> lock(_windowsMutex);

    Window* window = nullptr;
    for (const auto& w : _windows) {
        if (w && w->GetID() == windowID) {
            window = w.get();
            break;
        }
    }

    if (window) {
        currentWindow = windowID;
        window->MakeContextCurrent();
        spdlog::info("[WindowManager] Set current window to ID: {}", windowID);
    }
    else {
        throw std::runtime_error("Window with ID " + std::to_string(windowID) + " not found.");
    }
}

Window* WindowManager::GetWindowByTitle(const std::string& title) {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    for (const auto& window : _windows) {
        if (window && window->GetTitle() == title) {
            return window.get();
        }
    }
    return nullptr;
}

Window* WindowManager::GetCurrentWindow() {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    for (const auto& window : _windows) {
        if (window && window->GetID() == currentWindow) {
            return window.get();
        }
    }
    return nullptr;
}

std::string WindowManager::GetWindowTitle(int windowID) {
    std::lock_guard<std::mutex> lock(_windowsMutex);
    for (const auto& window : _windows) {
        if (window && window->GetID() == windowID) {
            return window->GetTitle();
        }
    }
    return "";
}