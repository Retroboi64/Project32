#pragma once

#ifndef CONTEXT_GUARD_H
#define CONTEXT_GUARD_H

#include "../common.h"

class Window;

class ContextGuard {
private:
    GLFWwindow* _prevContext;
    Window* _window;

public:
    explicit ContextGuard(Window* window);
    ~ContextGuard();

    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    ContextGuard(ContextGuard&&) = delete;
    ContextGuard& operator=(ContextGuard&&) = delete;
};

#endif