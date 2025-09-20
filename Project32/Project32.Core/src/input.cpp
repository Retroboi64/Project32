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
#include "input.h"
#include "engine.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

 // TODO: Refactor Input System to a class based system
namespace Input {
	Engine* _engine = nullptr;

    GLFWwindow* _window = nullptr;

    constexpr int MAX_KEYS = 512;
    bool _keyPressed[MAX_KEYS] = { false };
    bool _keyDown[MAX_KEYS] = { false };
    bool _keyDownLastFrame[MAX_KEYS] = { false };
    double _mouseX = 0.0, _mouseY = 0.0;
    double _lastMouseX = 0.0, _lastMouseY = 0.0;
    bool _firstMouse = true;
    bool _mouseLocked = true;

    void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
        {
            _mouseX = xpos;
            _mouseY = ypos;
        }
    }

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
            _engine->GetWindow()->ToggleFullscreen();
        }
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
            _mouseLocked = !_mouseLocked;
            glfwSetInputMode(window, GLFW_CURSOR,
                _mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }

    void Init(Engine* engine) {
        _engine = Engine::GetInstance();
        _window = _engine->GetWindowManager()->GetWindowByID(0)->GetGLFWwindow();
        //GLFWwindow* window = _engine->GetWindow()->GetGLFWwindow();
        glfwSetCursorPosCallback(_window, MouseCallback);
        glfwSetKeyCallback(_window, KeyCallback);
    }

    void Update() {
        for (int i = 0; i < MAX_KEYS; i++) {
            bool currentState = (glfwGetKey(_window, i) == GLFW_PRESS);
            _keyPressed[i] = (currentState && !_keyDownLastFrame[i]);
            _keyDown[i] = currentState;
            _keyDownLastFrame[i] = currentState;
        }
    }

    bool KeyPressed(int keycode) { return keycode < MAX_KEYS && _keyPressed[keycode]; }
    bool KeyDown(int keycode) { return keycode < MAX_KEYS && _keyDown[keycode]; }
    bool IsMouseLocked() { return _mouseLocked; }

    glm::vec2 GetMouseDelta() {
        if (_firstMouse) {
            _lastMouseX = _mouseX;
            _lastMouseY = _mouseY;
            _firstMouse = false;
            return glm::vec2(0.0f);
        }

        glm::vec2 delta = glm::vec2(_mouseX - _lastMouseX, _lastMouseY - _mouseY);
        _lastMouseX = _mouseX;
        _lastMouseY = _mouseY;
        return delta;
    }
}