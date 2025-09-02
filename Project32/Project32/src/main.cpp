#include "common.h"
#include "input.h"
#include "renderer.h"
#include "GL.h"
#include "game.h"

int main() {
    try {
        GL::Init(1920, 1080, "Enhanced Quake Movement");
        Game::Init();
        Renderer::Init();

        FrameTimer timer;

        std::cout << "Game initialized successfully!" << std::endl;

        while (GL::WindowIsOpen()) {
            timer.Update();
            float deltaTime = timer.GetDeltaTime();

            Input::Update();
            Game::Update(deltaTime);
            Renderer::RenderFrame();
            GL::SwapBuffersPollEvents();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    GL::Cleanup();
    return 0;
}