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
#include <glm/glm.hpp>

class Engine;
class Window;
struct GLFWwindow;

class Input {
private:
    Engine* _engine = nullptr;
    Window* _window = nullptr;
    GLFWwindow* _glfwWindow = nullptr;

    static constexpr int MAX_KEYS = 512;
    bool keyPressed[MAX_KEYS];
    bool keyDown[MAX_KEYS];
    bool keyDownLastFrame[MAX_KEYS];

    double mouseX = 0.0;
    double mouseY = 0.0;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool _firstMouse = true;
    bool _mouseLocked = false;

public:
    explicit Input(Window* window);
    ~Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void Init();
    void Update();

    bool KeyPressed(int keycode) const;
    bool KeyDown(int keycode) const;
    bool IsMouseLocked() const;
    void SetMouseLocked(bool locked);
    glm::vec2 GetMousePosition() const;
    glm::vec2 GetMouseDelta();

    Engine* GetEngine() const { return _engine; }
    Window* GetWindow() const { return _window; }
};