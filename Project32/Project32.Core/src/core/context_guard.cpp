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