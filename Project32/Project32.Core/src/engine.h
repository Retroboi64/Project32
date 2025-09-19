#pragma once
#include "common.h"
#include "window.h"

class Window;
class Engine {
private:
    std::unique_ptr<WindowManager> _windowManager;
	std::unique_ptr<Window> _window = nullptr;
    bool isRunning = false;
    int width, height;
    std::string title;

    int _ID = -1;

    void Init();
    void Update();

    static Engine* s_instance;

public:
    Engine(int width, int height, const std::string& title);
    ~Engine();

    void Run();
    void Shutdown();

	bool IsRunning() const { return isRunning; }

    Window* GetWindow() const { return _window.get(); }
	WindowManager* GetWindowManager() const { return _windowManager.get(); }

    static Engine* GetInstance() { return s_instance; }

	int GetID() const { return _ID; }
};