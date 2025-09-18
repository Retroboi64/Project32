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

#include "common.h"

#include "engine.h"
#include "renderer.h"
#include "input.h"
#include "window.h"

/*
    TODO: CHANGE THIS LATER this is just for testing and demo purposes
    this only allows one engine instance at a time
    multiple instances would require more careful management of static/global state
    So for now we just enforce a singleton pattern
*/
Engine* Engine::s_instance = nullptr; 

Engine::Engine(int width, int height, const std::string& title)
	: width(width), height(height), title(title), _ID(_ID++)
{
    s_instance = this;
    _window = std::make_unique<Window>(width, height, title);
    Init();
}

Engine::~Engine() {
    Shutdown();
    s_instance = nullptr;
}

void Engine::Init() {
    try {
        _window->Init();
        while (!_window->WindowIsOpen()) {
            glfwPollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        Renderer::Init();
        Input::Init();
        isRunning = true;
    }
    catch (const std::exception& e) {
        std::cerr << "[Engine::Init] Fatal error: " << e.what() << std::endl;
        Shutdown();
        throw;
    }
}

void Engine::Run() {
    FrameTimer timer;

    while (isRunning && _window->WindowIsOpen()) {
        timer.Update();
        float dt = timer.GetDeltaTime();

        _window->PollEvents();
        Input::Update();
        Renderer::RenderFrame();
        _window->SwapBuffers();

        if (Input::KeyPressed(Engine::GetInstance()->GetWindow()->KEY_ESCAPE)) {
			Shutdown();
        }
    }
}

void Engine::Shutdown() {
    if (!isRunning) return;

    Renderer::Cleanup();
    _window->Shutdown();

    isRunning = false;
}