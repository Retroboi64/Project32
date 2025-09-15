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

#include "input.h"
#include "common.h"

namespace Input {
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void Init();
    void Update();
    bool KeyPressed(int keycode);
    bool KeyDown(int keycode);
    bool IsMouseLocked();

    glm::vec2 GetMouseDelta();
}