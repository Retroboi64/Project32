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

#include "common.h"
#include "GL.h"
#include "game.h"
#include "renderer.h"
#include "input.h"

static bool isInitialized = false;
static std::string status = "Engine not initialized";

extern "C"
{
    __declspec(dllexport) bool EngineInit()
    {
        try {
            GL::Init(1920, 1080, "Project32");
            Game::Init();
            Renderer::Init();

            isInitialized = true;
            status = "Engine initialized successfully";
			return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            Renderer::Cleanup();
            GL::Cleanup();
            return false;
        }
    }

    __declspec(dllexport) void EngineUpdate()
    {
        if (!isInitialized)
        {
            status = "Engine not initialized. Update skipped.";
            return;
        }
        status = "Engine updated frame";

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

    __declspec(dllexport) void EngineShutdown()
    {
        if (isInitialized)
        {
            isInitialized = false;
            status = "Engine shut down";

            Renderer::Cleanup();
            GL::Cleanup();
        }
    }

    __declspec(dllexport) const char* EngineGetStatus()
    {
        return status.c_str();
    }
}
