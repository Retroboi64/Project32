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
#include "ui_app.h"
#include "window.h"
#include "engine.h"

EAUI::EAUI(const std::string& title, int width, int height, Engine* engine)
    : _isRunning(false), _appTitle(title), _engine(engine), _deltaTime(0.0f)
{
    spdlog::info("[EAUI] Creating external UI application: {}", title);

    try {
        GLFWwindow* shareContext = nullptr;
        if (_engine && _engine->GetMainWindow()) {
            shareContext = _engine->GetMainWindow()->GetGLFWwindow();
        }

        _window = std::make_unique<Window>(width, height, title, shareContext);

        if (!_window) {
            throw std::runtime_error("Failed to create window for EAUI");
        }

        spdlog::info("[EAUI] External UI window created successfully");
    }
    catch (const std::exception& e) {
        spdlog::error("[EAUI] Failed to create EAUI: {}", e.what());
        throw;
    }
}

EAUI::~EAUI() {
    spdlog::info("[EAUI] Destroying external UI application");
    Shutdown();
}

void EAUI::Init() {
    if (!_window) {
        throw std::runtime_error("Cannot initialize EAUI: window is null");
    }

    spdlog::info("[EAUI] Initializing external UI application");

    _window->MakeContextCurrent();
    _isRunning = true;

    OnInit();

    spdlog::info("[EAUI] External UI application initialized");
}

void EAUI::Run() {
    if (!_window) {
        spdlog::error("[EAUI] Cannot run: window is null");
        return;
    }

    if (!_isRunning) {
        Init();
    }

    spdlog::info("[EAUI] Starting external UI main loop");

    _timer.Update();

    while (_isRunning && _window->IsOpen()) {
        RunFrame();
    }

    spdlog::info("[EAUI] External UI main loop ended");
    Shutdown();
}

void EAUI::RunFrame() {
    if (!_window || !_isRunning) return;

    _timer.Update();
    _deltaTime = _timer.GetDeltaTime();

    glfwPollEvents();
    _window->PollEvents();

    if (!_window->IsOpen()) {
        _isRunning = false;
        return;
    }

    _window->MakeContextCurrent();
    OnUpdate(_deltaTime);

    _window->Clear(0.15f, 0.15f, 0.15f, 1.0f);
    OnRender();

    _window->BeginImGuiFrame();
    {
        BuildUI();
    }
    _window->EndImGuiFrame();

    _window->SwapBuffers();
}

void EAUI::Shutdown() {
    if (!_isRunning) return;

    spdlog::info("[EAUI] Shutting down external UI application");

    _isRunning = false;

    OnShutdown();

    if (_window) {
        _window->Shutdown();
        _window.reset();
    }

    spdlog::info("[EAUI] External UI application shutdown complete");
}