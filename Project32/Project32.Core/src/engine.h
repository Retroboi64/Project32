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
    bool isRunning = false;
    int width, height;
    std::string title;

    static int s_nextID;
    int _ID;

    void Init();

    static Engine* s_instance;

public:
    Engine(int width, int height, const std::string& title);
    ~Engine();

    void Run();
    void Shutdown();

    bool IsRunning() const { return isRunning; }

    Window* GetWindow() const {
        return _windowManager ? _windowManager->GetWindowByID(0) : nullptr;
    }
    WindowManager* GetWindowManager() const { return _windowManager.get(); }
    Renderer* GetRenderer() const { return _renderer.get(); }

    static Engine* GetInstance() { return s_instance; }
    int GetID() const { return _ID; }
};
