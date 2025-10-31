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
#include "../core/engine.h"
#include "../renderer/renderer.h"
#include "../core/input.h"
#include "../core/window.h"

ThreadPool::ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        _workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_queueMutex);
                    _condition.wait(lock, [this] {
                        return _stop.load() || !_tasks.empty();
                        });

                    if (_stop.load() && _tasks.empty()) {
                        return;
                    }

                    if (!_tasks.empty()) {
                        task = std::move(_tasks.front());
                        _tasks.pop();
                    }
                }

                if (task) {
                    task();
                }
            }
            });
    }
    spdlog::info("[ThreadPool] Created with {} worker threads", threads);
}

ThreadPool::~ThreadPool() {
    _stop.store(true);
    _condition.notify_all();

    for (std::thread& worker : _workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    spdlog::info("[ThreadPool] Destroyed");
}

void ThreadPool::Wait() {
    std::unique_lock<std::mutex> lock(_queueMutex);
    _condition.wait(lock, [this] { return _tasks.empty(); });
}

int Engine::s_nextID = 0;
std::unique_ptr<EngineManager> EngineManager::s_instance = nullptr;

Engine::Engine(const std::string& title)
    : title(title), _ID(s_nextID++)
{
    try {
        Init();
    }
    catch (const std::exception& e) {
        spdlog::error("[Engine::Constructor] Failed to initialize engine {}: {}", _ID, e.what());
        throw e;
    }
}

Engine::~Engine() {
    Shutdown();
}

void Engine::Init() {
    try {
        _windowManager = std::make_unique<WindowManager>();

        size_t threadCount = std::max(2u, std::thread::hardware_concurrency() - 2);
        _threadPool = std::make_unique<ThreadPool>(threadCount);

        _scriptSystem = std::make_unique<ScriptSystem>(this); 
        _scriptSystem->Init();  

        if (_windowManager->Count() == 0) {
            spdlog::warn("[Engine::Init] No windows in window manager for engine {}", _ID);
        }
        else if (_mainWindowID < 0) {
            spdlog::info("[Engine::Init] No main window set for engine {}, defaulting to window ID {}", _ID, _mainWindowID);
        }
        else {
            spdlog::info("[Engine::Init] Engine {} initialized successfully with main window ID {} (Total windows: {})",
                _ID, _mainWindowID, _windowManager->Count());
        }

        isRunning.store(true);
        std::cout << "[Engine::Init] Engine " << _ID << " initialized successfully with window ID " << _mainWindowID << std::endl;
    }
    catch (const std::exception& e) {
        spdlog::error("[Engine::Init] Fatal error in engine {}: {}", _ID, e.what());
        Shutdown();
        throw e;
    }
}

void Engine::StartUpdateThread() {
    if (_updateThread && _updateThread->joinable()) {
        spdlog::warn("[Engine::StartUpdateThread] Update thread already running for engine {}", _ID);
        return;
    }

    isRunning.store(true);
    _updateThread = std::make_unique<std::thread>(&Engine::UpdateLoop, this);
    spdlog::info("[Engine::StartUpdateThread] Started update thread for engine {}", _ID);
}

void Engine::StopUpdateThread() {
    isRunning.store(false);

    if (_updateThread && _updateThread->joinable()) {
        _frameCV.notify_all();
        _updateThread->join();
        _updateThread.reset();
        spdlog::info("[Engine::StopUpdateThread] Stopped update thread for engine {}", _ID);
    }
}

void Engine::UpdateLoop() {
    FrameTimer timer;
    spdlog::info("[Engine::UpdateLoop] Update thread started for engine {}", _ID);

    while (isRunning.load()) {
        timer.Update();
        float dt = timer.GetDeltaTime();
        _deltaTime.store(dt);

        {
            std::unique_lock<std::mutex> lock(_dataMutex);
            _frameCV.wait(lock, [this] { return _renderComplete.load() || !isRunning.load(); });

            if (!isRunning.load()) break;
        }

        if (!isPaused.load()) {
            Update(dt);
        }

        {
            std::lock_guard<std::mutex> lock(_dataMutex);
            _frameReady.store(true);
            _renderComplete.store(false);
        }
        _frameCV.notify_all();

        _frameCount.fetch_add(1);
    }

    spdlog::info("[Engine::UpdateLoop] Update thread exited for engine {}", _ID);
}

void Engine::Update(float dt) {
    if (_scriptSystem) {
        _scriptSystem->Update(dt);
    }
}

void Engine::FixedUpdate(float fixedDt) {
    if (_scriptSystem) {
        _scriptSystem->FixedUpdate(fixedDt);
    }
}

void Engine::RenderFrame() {
    if (!isRunning.load()) return;

    {
        std::unique_lock<std::mutex> lock(_dataMutex);
        _frameCV.wait_for(lock, std::chrono::milliseconds(16),
            [this] { return _frameReady.load() || !isRunning.load(); });

        if (!isRunning.load()) return;
        _frameReady.store(false);
    }

    int windowCount = _windowManager->Count();
    if (windowCount <= 0) {
        spdlog::info("[Engine::RenderFrame] No windows remaining, stopping engine {}", _ID);
        isRunning.store(false);
        return;
    }

    for (int i = windowCount - 1; i >= 0; --i) {
        Window* window = _windowManager->GetWindowAt(i);

        if (!window) {
            spdlog::error("[Engine::RenderFrame] Null window at index {}", i);
            continue;
        }

        if (!window->IsOpen()) {
            spdlog::info("[Engine::RenderFrame] Window {} closed, removing from manager", window->GetID());
            _windowManager->RemoveWindow(window->GetID());

            if (_windowManager->Count() == 0) {
                spdlog::info("[Engine::RenderFrame] Last window closed, stopping engine {}", _ID);
                isRunning.store(false);
                break;
            }
            continue;
        }

		window->GetRenderer()->RenderFrame();

        window->Render();
        window->SwapBuffers();
    }

    {
        std::lock_guard<std::mutex> lock(_dataMutex);
        _renderComplete.store(true);
    }
    _frameCV.notify_all();
}

void Engine::Run() {
    if (!isRunning.load()) {
        std::cerr << "[Engine::Run] Engine " << _ID << " is not running, cannot start main loop" << std::endl;
        return;
    }

    std::cout << "[Engine::Run] Starting main loop for engine " << _ID << std::endl;

    StartUpdateThread();

    while (isRunning.load()) {
        glfwPollEvents();
        RenderFrame();
    }

    spdlog::info("[Engine::Run] Main loop ended for engine {}", _ID);

    StopUpdateThread();
    Shutdown();
}

void Engine::Shutdown() {
    if (!isRunning.load() && !_windowManager) return;

    spdlog::info("[Engine::Shutdown] Shutting down engine {}", _ID);

    isRunning.store(false);
    StopUpdateThread();

    if (_scriptSystem) {
        _scriptSystem->Shutdown();
        _scriptSystem.reset();
    }

    if (_threadPool) {
        _threadPool.reset();
    }

    if (_windowManager) {
        _windowManager->RemoveAllWindows();
        _windowManager.reset();
    }

    _mainWindowID = -1;

    spdlog::info("[Engine::Shutdown] Engine {} shutdown complete", _ID);
}

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

int EngineManager::CreateEngine(const std::string& title) {
    try {
        auto engine = std::make_unique<Engine>(title);
        int engineID = engine->GetID();

        _engines.push_back(std::move(engine));

        if (_currentEngine == nullptr) {
            SetCurrentEngine(engineID);
        }

        spdlog::info("[EngineManager] Created engine {}: {}", engineID, title);
        return engineID;
    }
    catch (const std::exception& e) {
        spdlog::error("[EngineManager] Failed to create engine: {}", e.what());
        return -1;
    }
}

void EngineManager::RunAllEngines() {
    spdlog::info("[EngineManager] Starting main loop for all engines");

    if (_engines.empty()) {
        spdlog::error("[EngineManager] No engines to run!");
        return;
    }

    bool hasWindows = false;
    for (const auto& enginePtr : _engines) {
        if (enginePtr && enginePtr->GetWindowManager() && enginePtr->GetWindowManager()->Count() > 0) {
            hasWindows = true;
            break;
        }
    }

    if (!hasWindows) {
        spdlog::error("[EngineManager] No engines have windows to run!");
        return;
    }

    _running.store(true);

    for (auto& enginePtr : _engines) {
        if (enginePtr) {
            enginePtr->StartUpdateThread();
        }
    }

    FrameTimer timer;

    while (_running.load()) {
        timer.Update();
        float dt = timer.GetDeltaTime();

        glfwPollEvents();

        bool anyRunning = false;

        for (auto& enginePtr : _engines) {
            Engine* engine = enginePtr.get();
            if (!engine || !engine->IsRunning()) continue;

            anyRunning = true;
            engine->RenderFrame();
        }

        if (!anyRunning) {
            spdlog::info("[EngineManager] All engines have stopped running, exiting main loop");
            _running.store(false);
            break;
        }
    }

    spdlog::info("[EngineManager] Main loop exited, ensuring all engines are shutdown");

    for (auto& engine : _engines) {
        if (engine && engine->IsRunning()) {
            engine->Shutdown();
        }
    }

    glfwTerminate();
}

bool EngineManager::DestroyEngine(int engineID) {
    auto it = std::find_if(_engines.begin(), _engines.end(),
        [engineID](const std::unique_ptr<Engine>& engine) {
            return engine->GetID() == engineID;
        });

    if (it != _engines.end()) {
        if (_currentEngine && _currentEngine->GetID() == engineID) {
            _currentEngine = nullptr;
            _currentEngineID = -1;
        }

        if ((*it)->IsRunning()) {
            (*it)->Shutdown();
        }

        spdlog::info("[EngineManager] Destroying engine {}", engineID);
        _engines.erase(it);

        if (!_engines.empty() && _currentEngine == nullptr) {
            SetCurrentEngine(_engines[0]->GetID());
        }

        return true;
    }
    return false;
}

void EngineManager::DestroyAllEngines() {
    spdlog::info("[EngineManager] Destroying all engines");

    for (auto& engine : _engines) {
        if (engine && engine->IsRunning()) {
            engine->Shutdown();
        }
    }

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
        spdlog::info("[EngineManager] Set current engine to {}", engineID);
        return true;
    }

    spdlog::warn("[EngineManager] Could not set current engine to {} (not found)", engineID);
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
    spdlog::info("[EngineManager] Engine Count: {}", _engines.size());
    spdlog::info("[EngineManager] Current Engine: {}", _currentEngineID);

    for (const auto& engine : _engines) {
        spdlog::info("[EngineManager] Engine {}: {} (Frame: {}, DT: {:.3f}ms)",
            engine->GetID(),
            engine->IsRunning() ? "Running" : "Stopped",
            engine->GetFrameCount(),
            engine->GetDeltaTime() * 1000.0f);
    }
}