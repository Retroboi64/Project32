#pragma once
#include "common.h"
#include "window.h"

class Window;
class Renderer;
class Input;

class Engine {
private:
    std::unique_ptr<WindowManager> _windowManager;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<Input> _input;
    bool isRunning = false;
    int width, height;
    std::string title;

    static int s_nextID;
    int _ID;

    void Init();

public:
    Engine(int width, int height, const std::string& title);
    ~Engine();

    void Run();
    void Shutdown();

    bool IsRunning() const { return isRunning; }

    Window* GetWindow() const {
        return _windowManager ? _windowManager->GetWindowByID(_ID) : nullptr;
    }
    WindowManager* GetWindowManager() const { return _windowManager.get(); }
    Renderer* GetRenderer() const { return _renderer.get(); }
    Input* GetInput() const { return _input.get(); }

    int GetID() const { return _ID; }
};

class EngineManager {
private:
    std::vector<std::unique_ptr<Engine>> _engines;
    Engine* _currentEngine = nullptr;
    int _currentEngineID = -1;
    static std::unique_ptr<EngineManager> s_instance;

public:
    EngineManager() = default;
    ~EngineManager() = default;

    static EngineManager* Instance();
    static void DestroyInstance();

    int CreateEngine(int width, int height, const std::string& title);
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