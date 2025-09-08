#include "common.h"
#include "input.h"
#include "renderer.h"
#include "GL.h"
#include "game.h"

int main() {
    try {
        GL::Init(1920, 1080, "Project32");
        Game::Init();
        Renderer::Init();

        FrameTimer timer;
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
        Renderer::Cleanup();
        GL::Cleanup();
        return -1;
    }

	Renderer::Cleanup();
    GL::Cleanup();
    return 0;
}