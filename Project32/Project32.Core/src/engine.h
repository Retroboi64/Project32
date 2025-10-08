#pragma once
#include "common.h"
#include "window.h"

class Window;
class Renderer;
class Input;

class Engine {
private:
    std::unique_ptr<WindowManager> _windowManager;
    bool isRunning = false;
    std::string title;
    int _mainWindowID = -1;  

    static int s_nextID;
    int _ID;

    void Init();

public:
    Engine(const std::string& title);
    ~Engine();

    void Run();
    void Shutdown();

    bool IsRunning() const { return isRunning; }

    void SetMainWindow(int index) { _mainWindowID = index; }
    int GetMainWindowID() { return _mainWindowID; }

    Window* GetMainWindow() const { return _windowManager ? _windowManager->GetWindowByID(_mainWindowID) : nullptr; }
    WindowManager* GetWindowManager() const { return _windowManager.get(); }

    int GetID() const { return _ID; }
    int GetMainWindowID() const { return _mainWindowID; }
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