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