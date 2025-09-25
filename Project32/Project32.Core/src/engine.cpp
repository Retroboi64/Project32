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

int Engine::s_nextID = 0;
std::unique_ptr<EngineManager> EngineManager::s_instance = nullptr;

Engine::Engine(int width, int height, const std::string& title)
    : width(width), height(height), title(title), _ID(s_nextID++)
{
    try {
        _windowManager = std::make_unique<WindowManager>();
        int windowID = _windowManager->AddWindow(width, height, title);
        _renderer = std::make_unique<Renderer>(this);
        _input = std::make_unique<Input>(this); 
        Init();
    }
    catch (const std::exception& e) {
        std::cerr << "[Engine::Constructor] Failed to initialize engine " << _ID << ": " << e.what() << std::endl;
        throw;
    }
}

Engine::~Engine() {
    Shutdown();
}

void Engine::Init() {
    try {
        Window* mainWindow = _windowManager->GetWindowByID(_ID);
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

        _input->Init();

        isRunning = true;
        std::cout << "[Engine::Init] Engine " << _ID << " initialized successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[Engine::Init] Fatal error in engine " << _ID << ": " << e.what() << std::endl;
        Shutdown();
        throw;
    }
}

void Engine::Run() {
    if (!isRunning) {
        std::cerr << "[Engine::Run] Engine " << _ID << " is not running, cannot start main loop" << std::endl;
        return;
    }

    FrameTimer timer;
    Window* mainWindow = GetWindow();

    if (!mainWindow) {
        std::cerr << "[Engine::Run] No main window available for engine " << _ID << std::endl;
        return;
    }

    std::cout << "[Engine::Run] Starting main loop for engine " << _ID << std::endl;

    EngineManager::Instance()->SetCurrentEngine(_ID);

    while (isRunning && mainWindow->WindowIsOpen()) {
        timer.Update();
        float dt = timer.GetDeltaTime();

        mainWindow->PollEvents();

        _input->Update();

        if (_input->KeyPressed(GLFW_KEY_ESCAPE)) {
            std::cout << "[Engine::Run] Escape key pressed, shutting down engine " << _ID << std::endl;
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

    std::cout << "[Engine::Shutdown] Shutting down engine " << _ID << std::endl;

    if (_renderer) {
        _renderer->Cleanup();
    }

    if (_input) {
        _input.reset(); 
    }

    if (_windowManager) {
        Window* mainWindow = _windowManager->GetWindowByID(_ID);
        if (mainWindow) {
            // Window cleanup handled by WindowManager destructor
        }
    }

    isRunning = false;
    std::cout << "[Engine::Shutdown] Engine " << _ID << " shutdown complete" << std::endl;
}

// EngineManager Implementation
EngineManager* EngineManager::Instance() {
    if (!s_instance) {
        s_instance = std::make_unique<EngineManager>();
    }
    return s_instance.get();
}

void EngineManager::DestroyInstance() {
    if (s_instance) {
        s_instance->DestroyAllEngines();
        s_instance.reset();
    }
}

int EngineManager::CreateEngine(int width, int height, const std::string& title) {
    try {
        auto engine = std::make_unique<Engine>(width, height, title);
        int engineID = engine->GetID();

        _engines.push_back(std::move(engine));

        // Set as current engine if it's the first one
        if (_currentEngine == nullptr) {
            SetCurrentEngine(engineID);
        }

        std::cout << "[EngineManager] Created engine " << engineID << ": " << title << std::endl;
        return engineID;
    }
    catch (const std::exception& e) {
        std::cerr << "[EngineManager] Failed to create engine: " << e.what() << std::endl;
        return -1;
    }
}

bool EngineManager::DestroyEngine(int engineID) {
    auto it = std::find_if(_engines.begin(), _engines.end(),
        [engineID](const std::unique_ptr<Engine>& engine) {
            return engine->GetID() == engineID;
        });

    if (it != _engines.end()) {
        // If we're destroying the current engine, clear current
        if (_currentEngine && _currentEngine->GetID() == engineID) {
            _currentEngine = nullptr;
            _currentEngineID = -1;
        }

        std::cout << "[EngineManager] Destroying engine " << engineID << std::endl;
        _engines.erase(it);

        // If there are still engines, set the first one as current
        if (!_engines.empty() && _currentEngine == nullptr) {
            SetCurrentEngine(_engines[0]->GetID());
        }

        return true;
    }
    return false;
}

void EngineManager::DestroyAllEngines() {
    std::cout << "[EngineManager] Destroying all engines (" << _engines.size() << ")" << std::endl;
    _currentEngine = nullptr;
    _currentEngineID = -1;
    _engines.clear();
}

bool EngineManager::SetCurrentEngine(int engineID) {
    auto it = std::find_if(_engines.begin(), _engines.end(),
        [engineID](const std::unique_ptr<Engine>& engine) {
            return engine->GetID() == engineID;
        });

    if (it != _engines.end()) {
        _currentEngine = it->get();
        _currentEngineID = engineID;
        std::cout << "[EngineManager] Set current engine to " << engineID << std::endl;
        return true;
    }
    return false;
}

Engine* EngineManager::GetEngineByID(int engineID) const {
    auto it = std::find_if(_engines.begin(), _engines.end(),
        [engineID](const std::unique_ptr<Engine>& engine) {
            return engine->GetID() == engineID;
        });

    return (it != _engines.end()) ? it->get() : nullptr;
}

std::vector<Engine*> EngineManager::GetAllEngines() const {
    std::vector<Engine*> engines;
    engines.reserve(_engines.size());

    for (const auto& engine : _engines) {
        engines.push_back(engine.get());
    }

    return engines;
}

bool EngineManager::HasEngine(int engineID) const {
    return GetEngineByID(engineID) != nullptr;
}

void EngineManager::PrintEngineInfo() const {
    std::cout << "[EngineManager] Engine Count: " << _engines.size() << std::endl;
    std::cout << "[EngineManager] Current Engine: " << _currentEngineID << std::endl;

    for (const auto& engine : _engines) {
        std::cout << "  Engine " << engine->GetID() << ": "
            << (engine->IsRunning() ? "Running" : "Stopped") << std::endl;
    }
}