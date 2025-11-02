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
#include "context_guard.h"
#include "window.h"

ContextGuard::ContextGuard(Window* window)
    : _prevContext(glfwGetCurrentContext()), _window(window)
{
    if (_window) {
        _window->MakeContextCurrent();
    }
}

ContextGuard::~ContextGuard() {
    if (_prevContext && _prevContext != (_window ? _window->GetGLFWwindow() : nullptr)) {
        glfwMakeContextCurrent(_prevContext);
    }
}