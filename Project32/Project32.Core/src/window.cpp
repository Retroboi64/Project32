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

#include "common.h"
#include "window.h"

int Window::_nextID = 0;

static void StaticFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (window) {
        window->OnFramebufferResize(width, height);
    }
}

Window::Window(int width, int height, const std::string& title, GLFWwindow* shareContext = nullptr)
    : _width(width), _height(height), _title(title), _ID(_nextID++)
{
    std::cout << "[Window " << _ID << "] Constructor called: " << width << "x" << height << " '" << title << "'" << std::endl;
    std::cout.flush();

    std::cout << "[Window " << _ID << "] Step W1: Checking GLFW initialization..." << std::endl;
    std::cout.flush();

    static bool glfwInitialized = false;
    if (!glfwInitialized) {
        if (!glfwInit()) {
            std::cerr << "[Window " << _ID << "] Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Failed to initialize GLFW");
        }
        glfwInitialized = true;
        std::cout << "[Window " << _ID << "] GLFW initialized successfully" << std::endl;
    }
    else {
        std::cout << "[Window " << _ID << "] GLFW already initialized" << std::endl;
    }

    std::cout << "[Window " << _ID << "] Step W2: Setting window hints..." << std::endl;
    std::cout.flush();

    if (GraphicsBackend::GetCurrentType() == BackendType::OPENGL) {
        std::cout << "[Window " << _ID << "] Using OpenGL backend hints" << std::endl;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }
    else if (GraphicsBackend::GetCurrentType() == BackendType::UNDEFINED) {
        std::cout << "[Window " << _ID << "] No backend selected yet, defaulting to OpenGL hints" << std::endl;
    }
    else {
		std::cerr << "[Window " << _ID << "] Warning: Selected backend not supported for window creation. Defaulting to OpenGL hints." << std::endl;
    }

    std::cout << "[Window " << _ID << "] Step W3: Creating GLFW window..." << std::endl;
    std::cout.flush();

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, shareContext);
    if (!_window) {
        std::cerr << "[Window " << _ID << "] Failed to create GLFW window" << std::endl;
        throw std::runtime_error("Failed to create GLFW window");
    }

    std::cout << "[Window " << _ID << "] Step W4: GLFW window created at: " << _window << std::endl;
    std::cout.flush();

    glfwSetWindowUserPointer(_window, this);

    std::cout << "[Window " << _ID << "] Step W5: Making context current..." << std::endl;
    std::cout.flush();

    glfwMakeContextCurrent(_window);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(_window, StaticFramebufferSizeCallback);

    std::cout << "[Window " << _ID << "] Step W6: Initializing GLAD..." << std::endl;
    std::cout.flush();

    static bool gladInitialized = false;
    if (!gladInitialized) {
        InitGLAD();
        gladInitialized = true;
    }
    else {
        std::cout << "[Window " << _ID << "] GLAD already initialized" << std::endl;
    }

    std::cout << "[Window " << _ID << "] Step W7: Setting OpenGL state..." << std::endl;
    std::cout.flush();

    if (GraphicsBackend::GetCurrentType() == BackendType::UNDEFINED) {
        GraphicsBackend::Initialize(BackendType::OPENGL);

        glViewport(0, 0, width, height);

        std::cout << "[Renderer] Initialized graphics backend" << std::endl;
    }
    else {
        std::cout << "[Renderer] Using existing graphics backend" << std::endl;
    }

    SetVSync(_vsync);
    _isOpen = true;

    std::cout << "[Window " << _ID << "] Step W8: Initializing Input, UI and Renderer..." << std::endl;
    std::cout.flush();

    _input = std::make_unique<Input>(this);
    _input->Init();
    std::cout << "[Window " << _ID << "] Input initialized successfully" << std::endl;

    _ui = std::make_unique<UIX>(_window);
    std::cout << "[Window " << _ID << "] UI initialized successfully" << std::endl;

    _renderer = std::make_unique<Renderer>(this);
    _renderer->Init();
    std::cout << "[Window " << _ID << "] Renderer initialized successfully" << std::endl;

    std::cout << "[Window " << _ID << "] Created successfully: " << width << "x" << height << " '" << title << "'" << std::endl;
    std::cout << "[Window " << _ID << "] OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "[Window " << _ID << "] GPU: " << glGetString(GL_RENDERER) << std::endl;
    std::cout.flush();
}

Window::~Window() {
    std::cout << "[Window " << _ID << "] Destructor called" << std::endl;

    if (_input) {
        _input.reset();
        std::cout << "[Window " << _ID << "] Input cleaned up" << std::endl;
    }

    if (_renderer) {
        _renderer->Cleanup();
        _renderer.reset();
        std::cout << "[Window " << _ID << "] Renderer cleaned up" << std::endl;
    }

    if (_ui && _window) {
        _ui->Cleanup(_window);
        _ui.reset();
        std::cout << "[Window " << _ID << "] UI cleaned up" << std::endl;
    }

    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
        std::cout << "[Window " << _ID << "] GLFW window destroyed" << std::endl;
    }

    std::cout << "[Window " << _ID << "] Destructor completed" << std::endl;
}

void Window::Init() {
    std::cout << "[Window " << _ID << "] Init() called (initialization already done in constructor)" << std::endl;
}

void Window::InitGLAD() {
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD for Window " + std::to_string(_ID));
    }
    std::cout << "[Window " << _ID << "] GLAD initialized successfully" << std::endl;
}

void Window::Shutdown() {
    std::cout << "[Window " << _ID << "] Shutdown called" << std::endl;

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
        _isOpen = false;
        std::cout << "[Window " << _ID << "] Shutdown complete" << std::endl;
    }
}

void Window::Render() {
    if (!_window || !_renderer || !_renderer->IsReady()) {
        return;
    }

    MakeContextCurrent();
    _renderer->RenderFrame();
}

void Window::PollEvents() {
    glfwPollEvents();
    _isOpen = !glfwWindowShouldClose(_window);

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
    return _isOpen && _window && !glfwWindowShouldClose(_window);
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

    return (width, height);
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
		//GraphicsBackend::SetClearColor(r, g, b, a);
    }
}

void Window::OnFramebufferResize(int width, int height) {
    _width = width;
    _height = height;

    if (_window) {
        glfwMakeContextCurrent(_window);
        glViewport(0, 0, width, height);
    }

    if (_resizeCallback) {
        _resizeCallback(width, height);
    }
}

void Window::SetResizeCallback(std::function<void(int, int)> callback) {
    _resizeCallback = callback;
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::BeginImGuiFrame() {
    if (!_window || !_ui || !_ui->IsInitialized()) {
        std::cerr << "[Window " << _ID << "] Warning: BeginImGuiFrame called but ImGui not properly initialized!" << std::endl;
        return;
    }

    glfwMakeContextCurrent(_window);
    _ui->BeginImgui();
}

void Window::EndImGuiFrame() {
    if (!_window || !_ui || !_ui->IsInitialized()) {
        std::cerr << "[Window " << _ID << "] Warning: EndImGuiFrame called but ImGui not properly initialized!" << std::endl;
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
    return static_cast<int>(_windows.size());
}

Window* WindowManager::GetWindowAt(int index) {
    if (index >= 0 && index < static_cast<int>(_windows.size())) {
        return _windows[index].get();
    }
    return nullptr;
}

Window* WindowManager::GetWindowByID(int windowID) {
    for (const auto& window : _windows) {
        if (window->GetID() == windowID) {
            return window.get();
        }
    }
    return nullptr;
}

int WindowManager::AddWindow(int width, int height, const std::string& name) {
    try {
        GLFWwindow* shareContext = nullptr;

        if (!_windows.empty()) {
            shareContext = _windows.front()->GetGLFWwindow();
        }

        auto window = std::make_unique<Window>(width, height, name, shareContext);
        int windowID = window->GetID();
        _windows.push_back(std::move(window));
        std::cout << "[WindowManager] Added window with ID: " << windowID << " at index: " << (_windows.size() - 1) << std::endl;
        return windowID;
    }
    catch (const std::exception& e) {
        std::cerr << "[WindowManager] Failed to add window: " << e.what() << std::endl;
        return -1;
    }
}

int WindowManager::RemoveWindow(int windowID) {
    for (auto it = _windows.begin(); it != _windows.end(); ++it) {
        if ((*it)->GetID() == windowID) {
            std::cout << "[WindowManager] Removing window with ID: " << windowID << std::endl;
            _windows.erase(it);
            if (currentWindow == windowID) {
                currentWindow = -1;
            }
            return currentWindow;
        }
    }
    std::cerr << "[WindowManager] Window with ID " << windowID << " not found for removal" << std::endl;
    return -1;
}

void WindowManager::RemoveAllWindows() {
    std::cout << "[WindowManager] Removing all windows (" << _windows.size() << ")" << std::endl;
    _windows.clear();
    currentWindow = -1;
}

void WindowManager::Cleanup() {
    RemoveAllWindows();
}

void WindowManager::SetCurrentWindow(int windowID) {
    Window* window = GetWindowByID(windowID);
    if (window) {
        currentWindow = windowID;
        window->MakeContextCurrent();
        std::cout << "[WindowManager] Set current window to ID: " << windowID << std::endl;
    }
    else {
        throw std::runtime_error("Window with ID " + std::to_string(windowID) + " not found.");
    }
}

Window* WindowManager::GetWindowByTitle(const std::string& title) {
    for (const auto& window : _windows) {
        if (window->GetTitle() == title) {
            return window.get();
        }
    }
    return nullptr;
}

Window* WindowManager::GetCurrentWindow() {
    return GetWindowByID(currentWindow);
}

std::string WindowManager::GetWindowTitle(int windowID) {
    Window* window = GetWindowByID(windowID);
    return window ? window->GetTitle() : "";
}