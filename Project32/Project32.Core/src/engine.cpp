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

#include "engine.h"
#include "renderer.h"
#include "input.h"
#include "window.h"

Engine* Engine::s_instance = nullptr;
int Engine::s_nextID = 0;

Engine::Engine(int width, int height, const std::string& title)
    : width(width), height(height), title(title), _ID(s_nextID++)
{
    if (s_instance != nullptr) {
        throw std::runtime_error("Only one Engine instance is allowed at a time");
    }

    s_instance = this;

    try {
        _windowManager = std::make_unique<WindowManager>();
        int windowID = _windowManager->AddWindow(width, height, title);
        _renderer = std::make_unique<Renderer>();
        Init();
    }
    catch (const std::exception& e) {
        std::cerr << "[Engine::Constructor] Failed to initialize: " << e.what() << std::endl;
        s_instance = nullptr;
        throw;
    }
}

Engine::~Engine() {
    Shutdown();
    s_instance = nullptr;
}

void Engine::Init() {
    try {
        Window* mainWindow = _windowManager->GetWindowByID(0);
        if (!mainWindow) {
            throw std::runtime_error("Failed to get main window");
        }

        mainWindow->Init();

        while (!mainWindow->WindowIsOpen()) {
            glfwPollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        _renderer->Init();

        while (!_renderer->IsReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        Input::GetInstance()->Init();

        isRunning = true;
        std::cout << "[Engine::Init] Engine initialized successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[Engine::Init] Fatal error: " << e.what() << std::endl;
        Shutdown();
        throw;
    }
}

void Engine::Run() {
    if (!isRunning) {
        std::cerr << "[Engine::Run] Engine is not running, cannot start main loop" << std::endl;
        return;
    }

    FrameTimer timer;
    Window* mainWindow = GetWindow();

    if (!mainWindow) {
        std::cerr << "[Engine::Run] No main window available" << std::endl;
        return;
    }

    std::cout << "[Engine::Run] Starting main loop" << std::endl;

    while (isRunning && mainWindow->WindowIsOpen()) {
        timer.Update();
        float dt = timer.GetDeltaTime();

        mainWindow->PollEvents();

        Input::GetInstance()->Update();

        if (Input::GetInstance()->KeyPressed(GLFW_KEY_ESCAPE)) {
            std::cout << "[Engine::Run] Escape key pressed, shutting down" << std::endl;
            break;
        }

        if (_renderer) {
            _renderer->RenderFrame();
        }

        mainWindow->SwapBuffers();
    }

    Shutdown();
}

void Engine::Shutdown() {
    if (!isRunning) return;

    std::cout << "[Engine::Shutdown] Shutting down engine" << std::endl;

    if (_renderer) {
        _renderer->Cleanup();
    }

    // Note: Input cleanup is handled by its destructor
    if (_windowManager) {
        Window* mainWindow = _windowManager->GetWindowByID(0);
        if (mainWindow) {
            // Window cleanup handled by WindowManager destructor
        }
    }

    isRunning = false;
    std::cout << "[Engine::Shutdown] Engine shutdown complete" << std::endl;
}