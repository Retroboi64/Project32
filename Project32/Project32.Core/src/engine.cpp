#include "engine.h"
#include "GL.h"
#include "game.h"
#include "renderer.h"
#include "input.h"

Engine::Engine(int width, int height, const std::string& title)
    : width(width), height(height), title(title)
{
    Init();
}

Engine::~Engine() {
    Shutdown();
}

void Engine::Init() {
    try {
        GL::Init(width, height, title);
        Renderer::Init();
        Game::Init();
        isRunning = true;
    }
    catch (const std::exception& e) {
        std::cerr << "[Engine::Init] Fatal error: " << e.what() << std::endl;
        Shutdown();
        throw; // rethrow so caller knows init failed
    }
}

void Engine::Run() {
    FrameTimer timer;

    while (isRunning && GL::WindowIsOpen()) {
        timer.Update();
        float dt = timer.GetDeltaTime();

        Input::Update();
        Game::Update(dt);
        Renderer::RenderFrame();

        GL::SwapBuffersPollEvents();
    }
}

void Engine::Shutdown() {
    if (!isRunning) return;

    // Game::Cleanup();
    Renderer::Cleanup();
    GL::Cleanup();

    isRunning = false;
}
