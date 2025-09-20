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

std::vector<std::unique_ptr<Window>> s_windows; // Testing multiple windows
std::unique_ptr<WindowManager> s_windowManager = nullptr;
std::unique_ptr<Renderer> s_renderer = nullptr;

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
	s_windowManager = std::make_unique<WindowManager>();
	int windowID = s_windowManager->AddWindow(width, height, title);
	s_renderer = std::make_unique<Renderer>();
    Init();
}

Engine::~Engine() {
    Shutdown();
    s_instance = nullptr;
}

void Engine::Init() {
    try {
		// TODO: Input Does Not Initialize Properly
		s_windowManager->GetWindowByID(0)->Init();
        while (!s_windowManager->GetWindowByID(0)->WindowIsOpen()) {
            glfwPollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        s_renderer->Init();
        while (!s_renderer->IsReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        Input::Init(s_instance);
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

    while (isRunning && s_windowManager->GetWindowByID(0)->WindowIsOpen()) {
        timer.Update();
        float dt = timer.GetDeltaTime();

        s_windowManager->GetWindowByID(0)->PollEvents();
		s_renderer->RenderFrame();
        s_windowManager->GetWindowByID(0)->SwapBuffers();
        Input::Update();

        if (Input::KeyPressed(Engine::GetInstance()->GetWindowManager()->GetWindowByID(0)->KEY_ESCAPE)) {
			Shutdown();
        }
    }
}

void Engine::Shutdown() {
    if (!isRunning) return;

	s_renderer->Cleanup();
    //_window->Shutdown();

    isRunning = false;
}