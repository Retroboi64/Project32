#pragma once
#include "common.h"
#include "window.h"

class Window;
class Engine {
public:
    Engine(int width, int height, const std::string& title);
    ~Engine();
    void Run();
    void Shutdown();
    Window* GetWindow() const { return _window.get(); }

    static Engine* GetInstance() { return s_instance; }

private:
    std::unique_ptr<Window> _window = nullptr;
    bool isRunning = false;
    int width, height;
    std::string title;
    void Init();
    void Update();

    static Engine* s_instance;
};