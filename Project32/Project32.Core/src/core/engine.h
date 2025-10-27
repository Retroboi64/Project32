
#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include "../common.h"
#include "window.h"

class ThreadPool {
private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::mutex _queueMutex;
    std::condition_variable _condition;
    std::atomic<bool> _stop{ false };

public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F>
    void Enqueue(F&& f) {
        {
            std::lock_guard<std::mutex> lock(_queueMutex);
            _tasks.emplace(std::forward<F>(f));
        }
        _condition.notify_one();
    }

    void Wait();
    size_t GetThreadCount() const { return _workers.size(); }
};

class Window;
class Renderer;
class Input;

class Engine {
private:
    std::unique_ptr<WindowManager> _windowManager;
    std::atomic<bool> isRunning{ false };
    std::atomic<bool> isPaused{ false };
    std::string title;
    int _mainWindowID = -1;

    static int s_nextID;
    int _ID;

    std::unique_ptr<std::thread> _updateThread;
    std::unique_ptr<ThreadPool> _threadPool;  
    std::mutex _dataMutex;
    std::condition_variable _frameCV;
    std::atomic<bool> _frameReady{ false };
    std::atomic<bool> _renderComplete{ true };

    std::atomic<float> _deltaTime{ 0.0f };
    std::atomic<uint64_t> _frameCount{ 0 };

    void Init();
    void UpdateLoop(); 

public:
    Engine(const std::string& title);
    ~Engine();

    void StartUpdateThread();
    void StopUpdateThread();

    void Update(float dt);    
    void RenderFrame();       

    void Run();
    void Shutdown();

    bool IsRunning() const { return isRunning.load(); }
    bool IsPaused() const { return isPaused.load(); }
    void SetPaused(bool paused) { isPaused.store(paused); }

    void SetMainWindow(int index) { _mainWindowID = index; }
    int GetMainWindowID() { return _mainWindowID; }

    Window* GetMainWindow() const {
        return _windowManager ? _windowManager->GetWindowByID(_mainWindowID) : nullptr;
    }
    WindowManager* GetWindowManager() const { return _windowManager.get(); }
    ThreadPool* GetThreadPool() const { return _threadPool.get(); }

    int GetID() const { return _ID; }
    float GetDeltaTime() const { return _deltaTime.load(); }
    uint64_t GetFrameCount() const { return _frameCount.load(); }
};

class EngineManager {
private:
    std::vector<std::unique_ptr<Engine>> _engines;
    Engine* _currentEngine = nullptr;
    int _currentEngineID = -1;
    static std::unique_ptr<EngineManager> s_instance;

    std::atomic<bool> _running{ false };

public:
    EngineManager() = default;
    ~EngineManager() = default;

    static EngineManager* Instance();
    static void DestroyInstance();

    void RunAllEngines();

    int CreateEngine(const std::string& title);
    bool DestroyEngine(int engineID);
    void DestroyAllEngines();

    bool SetCurrentEngine(int engineID);
    Engine* GetCurrentEngine() const { return _currentEngine; }
    int GetCurrentEngineID() const { return _currentEngineID; }

    Engine* GetEngineByID(int engineID) const;
    std::vector<Engine*> GetAllEngines() const;
    size_t GetEngineCount() const { return _engines.size(); }

    bool HasEngine(int engineID) const;
    void PrintEngineInfo() const;
};

#endif // ENGINE_H