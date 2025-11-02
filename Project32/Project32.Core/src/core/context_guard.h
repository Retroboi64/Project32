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