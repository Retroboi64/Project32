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

// TODO: Move all OpenGL/GLAD and Imgui To A Better Place

#include "window.h"

int Window::_nextID = 0;

static void StaticFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (window) {
        window->OnFramebufferResize(width, height);
    }
}

Window::Window(int width, int height, const std::string& title)
    : _width(width), _height(height), _title(title), _ID(_nextID++) //, _imguiContext(nullptr), _imguiInitialized(false)
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    std::cout << "[Window " << _ID << "] Step W3: Creating GLFW window..." << std::endl;
    std::cout.flush();

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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

    InitGLAD();

    std::cout << "[Window " << _ID << "] Step W7: Setting OpenGL state..." << std::endl;
    std::cout.flush();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glViewport(0, 0, width, height);

    SetVSync(_vsync);
    _isOpen = true;

    std::cout << "[Window " << _ID << "] Step W8: Initializing ImGui..." << std::endl;
    std::cout.flush();

    _ui = std::make_unique<UIX>(_window);

    std::cout << "[Window " << _ID << "] Created successfully: " << width << "x" << height << " '" << title << "'" << std::endl;
    std::cout << "[Window " << _ID << "] OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "[Window " << _ID << "] GPU: " << glGetString(GL_RENDERER) << std::endl;
    std::cout.flush();
}

Window::~Window() {
    std::cout << "[Window " << _ID << "] Destructor called" << std::endl;

    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
        std::cout << "[Window " << _ID << "] GLFW window destroyed" << std::endl;
    }

    std::cout << "[Window " << _ID << "] Destructor completed" << std::endl;
}

void Window::Init() {}

void Window::InitGLAD() {
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD for Window " + std::to_string(_ID));
    }
    std::cout << "[Window " << _ID << "] GLAD initialized successfully" << std::endl;
}

void Window::Shutdown() {
    std::cout << "[Window " << _ID << "] Shutdown called" << std::endl;

    if (_window) {
        _ui->Cleanup(_window);

        glfwDestroyWindow(_window);
        _window = nullptr;
        _isOpen = false;
        std::cout << "[Window " << _ID << "] Shutdown complete" << std::endl;
    }
}

void Window::PollEvents() {
    glfwPollEvents();
    _isOpen = !glfwWindowShouldClose(_window);
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

void Window::SetSize(int width, int height) {
    if (_window) {
        glfwSetWindowSize(_window, width, height);
    }
    _width = width;
    _height = height;
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
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


int WindowManager::Count() {
    return static_cast<int>(_windows.size());
}

int WindowManager::AddWindow(int width, int height, const std::string& name) {
    try {
        auto window = std::make_unique<Window>(width, height, name);
        int windowID = window->GetID();
        _windows.push_back(std::move(window));
        std::cout << "[WindowManager] Added window with ID: " << windowID << std::endl;
        return windowID;
    }
    catch (const std::exception& e) {
        std::cerr << "[WindowManager] Failed to add window: " << e.what() << std::endl;
        return -1;
    }
}

int WindowManager::RemoveWindow(int index) {
    for (auto it = _windows.begin(); it != _windows.end(); ++it) {
        if ((*it)->GetID() == index) {
            std::cout << "[WindowManager] Removing window with ID: " << index << std::endl;
            _windows.erase(it);
            if (currentWindow == index) {
                currentWindow = -1;
            }
            return currentWindow;
        }
    }
    std::cerr << "[WindowManager] Window with ID " << index << " not found for removal" << std::endl;
    return -1;
}

void WindowManager::RemoveAllWindows() {
    _windows.clear();
}

void WindowManager::Cleanup() {
    RemoveAllWindows();
}

void WindowManager::SetCurrentWindow(int index) {
    for (int i = 0; i < Count(); i++) {
        if (_windows[i]->GetID() == index) {
            currentWindow = index;
            _windows[i]->MakeContextCurrent();
            std::cout << "[WindowManager] Set current window to ID: " << index << std::endl;
            return;
        }
    }
    throw std::runtime_error("Window with ID " + std::to_string(index) + " not found.");
}

Window* WindowManager::GetWindowByID(int index) {
    for (int i = 0; i < Count(); i++) {
        if (_windows[i]->GetID() == index) {
            return _windows[i].get();
        }
    }
    return nullptr;
}

Window* WindowManager::GetWindowByTitle(const std::string& title) {
    for (int i = 0; i < Count(); i++) {
        if (_windows[i]->GetTitle() == title) {
            return _windows[i].get();
        }
    }
    return nullptr;
}

Window* WindowManager::GetCurrentWindow() {
    return GetWindowByID(currentWindow);
}

std::string WindowManager::GetWindowTitle(int index) {
    for (int i = 0; i < Count(); i++) {
        if (_windows[i]->GetID() == index) {
            return _windows[i]->GetTitle();
        }
    }
    return "";
}