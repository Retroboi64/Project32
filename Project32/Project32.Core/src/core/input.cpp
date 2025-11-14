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
#include "input.h"
#include "engine.h"
#include "window.h"

static std::unordered_map<GLFWwindow*, Input*> g_windowInputMap;

static thread_local bool g_inMouseCallback = false;
static thread_local bool g_inKeyCallback = false;

Input::Input(Window* window) : _engine(nullptr), _window(window) {
    std::fill(std::begin(keyPressed), std::end(keyPressed), false);
    std::fill(std::begin(keyDown), std::end(keyDown), false);
    std::fill(std::begin(keyDownLastFrame), std::end(keyDownLastFrame), false);
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (g_inMouseCallback) {
        return;
    }
    g_inMouseCallback = true;

    ImGuiContext* currentContext = ImGui::GetCurrentContext();
    if (currentContext) {
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    }

    auto it = g_windowInputMap.find(window);
    if (it != g_windowInputMap.end()) {
        Input* input = it->second;
        input->mouseX = xpos;
        input->mouseY = ypos;
    }

    g_inMouseCallback = false;
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_inKeyCallback) {
        return;
    }
    g_inKeyCallback = true;

    ImGuiContext* currentContext = ImGui::GetCurrentContext();
    if (currentContext) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard && key != GLFW_KEY_F11 && key != GLFW_KEY_F1) {
            g_inKeyCallback = false;
            return;
        }
    }

    auto it = g_windowInputMap.find(window);
    if (it != g_windowInputMap.end()) {
        Input* input = it->second;

        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_F11) {
                if (input->_window) {
                    input->_window->ToggleFullscreen();
                }
            }
            else if (key == GLFW_KEY_F1) {
                input->_mouseLocked = !input->_mouseLocked;
                glfwSetInputMode(window, GLFW_CURSOR,
                    input->_mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            }
        }
    }

    g_inKeyCallback = false;
}

void Input::Init() {
    if (_engine) {
        if (!_window) {
            throw std::runtime_error("window not available for Input initialization");
        }

        _glfwWindow = _window->GetGLFWwindow();
        if (!_glfwWindow) {
            throw std::runtime_error("GLFW window not available for Input initialization");
        }

        g_windowInputMap[_glfwWindow] = this;
        glfwSetCursorPosCallback(_glfwWindow, MouseCallback);
        glfwSetKeyCallback(_glfwWindow, KeyCallback);
        glfwSetInputMode(_glfwWindow, GLFW_CURSOR,
            _mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

		spdlog::info("[Input::Init] Input system initialized for engine {}", _engine->GetID());
    }
    else if (_window) {
        _glfwWindow = _window->GetGLFWwindow();
        if (!_glfwWindow) {
            throw std::runtime_error("GLFW window not available for Input initialization");
        }

        g_windowInputMap[_glfwWindow] = this;
        glfwSetCursorPosCallback(_glfwWindow, MouseCallback);
        glfwSetKeyCallback(_glfwWindow, KeyCallback);
        glfwSetInputMode(_glfwWindow, GLFW_CURSOR,
            _mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        spdlog::info("[Input::Init] Input system initialized for window {}", _window->GetID());
    }
    else {
        throw std::runtime_error("Neither Engine nor Window instance available for Input initialization");
    }
}

void Input::Update() {
    if (!_glfwWindow) return;

    for (int i = 0; i < MAX_KEYS; i++) {
        bool currentState = (glfwGetKey(_glfwWindow, i) == GLFW_PRESS);
        keyPressed[i] = (currentState && !keyDownLastFrame[i]);
        keyDown[i] = currentState;
        keyDownLastFrame[i] = currentState;
    }
}

bool Input::KeyPressed(int keycode) const {
    return keycode >= 0 && keycode < MAX_KEYS && keyPressed[keycode];
}

bool Input::KeyDown(int keycode) const {
    return keycode >= 0 && keycode < MAX_KEYS && keyDown[keycode];
}

bool Input::IsMouseLocked() const {
    return _mouseLocked;
}

void Input::SetMouseLocked(bool locked) {
    _mouseLocked = locked;
    if (_glfwWindow) {
        glfwSetInputMode(_glfwWindow, GLFW_CURSOR,
            _mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

glm::vec2 Input::GetMousePosition() const {
    return glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
}

glm::vec2 Input::GetMouseDelta() {
    if (_firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        _firstMouse = false;
        return glm::vec2(0.0f);
    }

    glm::vec2 delta = glm::vec2(
        static_cast<float>(mouseX - lastMouseX),
        static_cast<float>(lastMouseY - mouseY)
    );
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    return delta;
}

Input::~Input() {
    if (_glfwWindow) {
        g_windowInputMap.erase(_glfwWindow);

        glfwSetCursorPosCallback(_glfwWindow, nullptr);
        glfwSetKeyCallback(_glfwWindow, nullptr);
    }

    std::string contextInfo;
    if (_engine) {
        contextInfo = "engine " + std::to_string(_engine->GetID());
    }
    else if (_window) {
        contextInfo = "window " + std::to_string(_window->GetID());
    }
    else {
        contextInfo = "unknown";
    }

	spdlog::info("[Input::~Input] Input system cleaned up for {}", contextInfo);
}