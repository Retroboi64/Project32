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

#ifndef UI_APP_H
#define UI_APP_H

#include "../common.h"
#include "window.h"

class EAUI {
private:
    std::unique_ptr<Window> _window;
    bool _isRunning;
    std::string _appTitle;

    class Engine* _engine;

    FrameTimer _timer;
    float _deltaTime;

public:
    explicit EAUI(const std::string& title = "Tool Window",
        int width = 1280,
        int height = 720,
        Engine* engine = nullptr);
    ~EAUI();

    void Init();
    void Run();     
    void RunFrame(); 
    void Shutdown();

    virtual void OnInit() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnRender() {}
    virtual void OnShutdown() {}
    virtual void BuildUI() {}

    bool IsRunning() const { return _isRunning; }
    void Close() { _isRunning = false; }

    Window* GetWindow() const { return _window.get(); }
    Engine* GetEngine() const { return _engine; }
    float GetDeltaTime() const { return _deltaTime; }

    UIX* GetUIX() const { return _window ? _window->GetUI() : nullptr; }
    Renderer* GetRenderer() const { return _window ? _window->GetRenderer() : nullptr; }
    Input* GetInput() const { return _window ? _window->GetInput() : nullptr; }
};

#endif // UI_APP_H