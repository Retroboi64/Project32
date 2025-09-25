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

#include "input.h"
#include "engine.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

static std::unordered_map<GLFWwindow*, Input*> g_windowInputMap;

Input::Input(Engine* engine) : _engine(engine) {
    std::fill(std::begin(keyPressed), std::end(keyPressed), false);
    std::fill(std::begin(keyDown), std::end(keyDown), false);
    std::fill(std::begin(keyDownLastFrame), std::end(keyDownLastFrame), false);
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    auto it = g_windowInputMap.find(window);
    if (it != g_windowInputMap.end()) {
        Input* input = it->second;
        input->mouseX = xpos;
        input->mouseY = ypos;
    }
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    auto it = g_windowInputMap.find(window);
    if (it != g_windowInputMap.end()) {
        Input* input = it->second;

        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_F11) {
                if (input->_engine && input->_engine->GetWindow()) {
                    input->_engine->GetWindow()->ToggleFullscreen();
                }
            }
            else if (key == GLFW_KEY_F1) {
                input->_mouseLocked = !input->_mouseLocked;
                glfwSetInputMode(window, GLFW_CURSOR,
                    input->_mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            }
        }
    }
}

void Input::Init() {
    if (!_engine) {
        throw std::runtime_error("Engine instance not available for Input initialization");
    }

    Window* mainWindow = _engine->GetWindow();
    if (!mainWindow) {
        throw std::runtime_error("Main window not available for Input initialization");
    }

    _window = mainWindow->GetGLFWwindow();
    if (!_window) {
        throw std::runtime_error("GLFW window not available for Input initialization");
    }

    // Register this Input instance with the window
    g_windowInputMap[_window] = this;

    glfwSetCursorPosCallback(_window, MouseCallback);
    glfwSetKeyCallback(_window, KeyCallback);

    glfwSetInputMode(_window, GLFW_CURSOR,
        _mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    std::cout << "[Input::Init] Input system initialized for engine " << _engine->GetID() << std::endl;
}

void Input::Update() {
    if (!_window) return;

    for (int i = 0; i < MAX_KEYS; i++) {
        bool currentState = (glfwGetKey(_window, i) == GLFW_PRESS);
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
    if (_window) {
        glfwSetInputMode(_window, GLFW_CURSOR,
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
    if (_window) {
        g_windowInputMap.erase(_window);

        glfwSetCursorPosCallback(_window, nullptr);
        glfwSetKeyCallback(_window, nullptr);
    }
    std::cout << "[Input::~Input] Input system cleaned up for engine "
        << (_engine ? std::to_string(_engine->GetID()) : "unknown") << std::endl;
}