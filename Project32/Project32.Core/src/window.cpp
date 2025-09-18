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

/*
	TODO: Move all OpenGL/GLAD and Imgui To A Better Place
*/

#include "window.h"
#include <iostream>

// Window Implamentation
static void StaticFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (window) {
        window->OnFramebufferResize(width, height);
    }
}

Window::Window(int width, int height, const std::string& title)
    : _width(width), _height(height), _title(title), _ID(_ID++)
{

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(_window, this);

    glfwMakeContextCurrent(_window);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(_window, StaticFramebufferSizeCallback);

    InitGLAD();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glViewport(0, 0, width, height);

    SetVSync(_vsync);
    _isOpen = true;

	InitImGui();

    std::cout << "Window " << _ID << " created: " << width << "x" << height << " '" << title << "'" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
}

Window::~Window() {
    if (_window) {
        glfwDestroyWindow(_window);
        std::cout << "Window " << _ID << " destroyed" << std::endl;
    }
    glfwTerminate();
}

void Window::Init() {
    // Placeholder for any future initialization code
}

void Window::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    float xscale, yscale;
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
    float main_scale = (xscale + yscale) * 0.5f;
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Window::InitGLAD() {
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

void Window::Shutdown() {
    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
        _isOpen = false;
        std::cout << "Window " << _ID << " shutdown" << std::endl;
    }
    glfwTerminate();
}

void Window::PollEvents() {
    glfwPollEvents();
    _isOpen = !glfwWindowShouldClose(_window);
}

void Window::SwapBuffers() {
    glfwSwapBuffers(_window);
}

bool Window::IsOpen() const {
    return _isOpen && !glfwWindowShouldClose(_window);
}

void Window::SetShouldClose(bool value) {
    glfwSetWindowShouldClose(_window, value);
    if (value) {
        _isOpen = false;
    }
}

void Window::GetSize(int& width, int& height) const {
    glfwGetWindowSize(_window, &width, &height);
}

glm::ivec2 Window::GetSize() const {
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    return glm::ivec2(width, height);
}

void Window::SetSize(int width, int height) {
    glfwSetWindowSize(_window, width, height);
    _width = width;
    _height = height;
}

void Window::GetPosition(int& x, int& y) const {
    glfwGetWindowPos(_window, &x, &y);
}

glm::ivec2 Window::GetPosition() const {
    int x, y;
    glfwGetWindowPos(_window, &x, &y);
    return glm::ivec2(x, y);
}

void Window::SetPosition(int x, int y) {
    glfwSetWindowPos(_window, x, y);
}

void Window::SetVSync(bool enabled) {
    _vsync = enabled;
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::ToggleFullscreen() {
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
    glfwSetWindowTitle(_window, title.c_str());
}

void Window::SetIcon(const std::string& iconPath) {
    _iconPath = iconPath;
}

void Window::MakeContextCurrent() {
    glfwMakeContextCurrent(_window);
}

void Window::SetCursorMode(CursorMode mode) {
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
    return glfwGetKey(_window, key) == GLFW_PRESS;
}

bool Window::IsMouseButtonPressed(int button) const {
    return glfwGetMouseButton(_window, button) == GLFW_PRESS;
}

glm::vec2 Window::GetMousePosition() const {
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Window::SetMousePosition(float x, float y) {
    glfwSetCursorPos(_window, x, y);
}

float Window::GetAspectRatio() const {
    return static_cast<float>(_width) / static_cast<float>(_height);
}

void Window::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::OnFramebufferResize(int width, int height) {
    _width = width;
    _height = height;
    glViewport(0, 0, width, height);

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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::EndImGuiFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// WindowManager Implamentation
int WindowManager::Count() {
    return static_cast<int>(_windows.size());
}

int WindowManager::AddWindow(int width, int height, const std::string& name) {
    int s = static_cast<int>(_windows.size());
    _windows.push_back(std::make_unique<Window>(width, height, name));
    return s;
}

int WindowManager::RemoveWindow(int index) {
    for (auto it = _windows.begin(); it != _windows.end(); ++it) {
        if ((*it)->GetID() == index) {
            _windows.erase(it);
            if (currentWindow == index) {
                currentWindow = -1;
			}
			return currentWindow;
        }
    }
	return -1;
}

void WindowManager::SetCurrentWindow(int index) {
    for (int i = 0; i < Count(); i++) {
        if (_windows[i]->GetID() == index) {
            currentWindow = index;
            _windows[i]->MakeContextCurrent();
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

std::string WindowManager::GetWindowTitle(int index) {
    for (int i = 0; i < Count(); i++) {
        if (_windows[i]->GetID() == index) {
            return _windows[i]->GetTitle();
        }
    }
    return "";
}