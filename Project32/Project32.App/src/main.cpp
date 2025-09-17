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

#include <windows.h>
#include <iostream>
#include <string>

 // Engine function types
typedef bool (*EngineInitFunc)(int width, int height, const char* title);
typedef void (*EngineRunFunc)();
typedef void (*EngineShutdownFunc)();
typedef bool (*EngineIsRunningFunc)();

// Input function types
typedef bool (*KeyPressedFunc)(int key);
typedef bool (*KeyDownFunc)(int key);
typedef void (*GetMousePosFunc)(float* x, float* y);
typedef void (*GetMouseDeltaFunc)(float* dx, float* dy);

// Game state
struct GameState {
    float cameraX = 0.0f, cameraY = 1.0f, cameraZ = 5.0f;
    float yaw = 0.0f, pitch = 0.0f;
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.002f;
    bool wireframe = false;
} game;

// Engine function pointers
struct EngineFunctions {
    EngineInitFunc Init;
    EngineRunFunc Run;
    EngineShutdownFunc Shutdown;
    EngineIsRunningFunc IsRunning;
    KeyPressedFunc KeyPressed;
    KeyDownFunc KeyDown;
    GetMousePosFunc GetMousePos;
    GetMouseDeltaFunc GetMouseDelta;
} engine;

// Game callbacks that the engine will call
extern "C" {
    __declspec(dllexport) void GameInit() {
        std::cout << "=== Game Initialized ===" << std::endl;
        std::cout << "WASD: Move around" << std::endl;
        std::cout << "Mouse: Look around" << std::endl;
        std::cout << "Space/Shift: Up/Down" << std::endl;
        std::cout << "M: Toggle wireframe" << std::endl;
        std::cout << "ESC: Exit" << std::endl;
        std::cout << "=========================" << std::endl;
    }

    __declspec(dllexport) void GameUpdate(float deltaTime) {
        // Handle input
        if (engine.KeyPressed(VK_ESCAPE)) {
            PostQuitMessage(0);
            return;
        }

        if (engine.KeyPressed('M')) {
            game.wireframe = !game.wireframe;
            std::cout << "Wireframe: " << (game.wireframe ? "ON" : "OFF") << std::endl;
        }

        // Mouse look
        float dx, dy;
        engine.GetMouseDelta(&dx, &dy);

        game.yaw += dx * game.mouseSensitivity;
        game.pitch -= dy * game.mouseSensitivity;

        // Clamp pitch
        if (game.pitch > 1.5f) game.pitch = 1.5f;
        if (game.pitch < -1.5f) game.pitch = -1.5f;

        // Movement
        float speed = game.moveSpeed * deltaTime;

        if (engine.KeyDown('W')) {
            game.cameraX += cos(game.yaw) * speed;
            game.cameraZ += sin(game.yaw) * speed;
        }
        if (engine.KeyDown('S')) {
            game.cameraX -= cos(game.yaw) * speed;
            game.cameraZ -= sin(game.yaw) * speed;
        }
        if (engine.KeyDown('A')) {
            game.cameraX -= cos(game.yaw + 1.57f) * speed;
            game.cameraZ -= sin(game.yaw + 1.57f) * speed;
        }
        if (engine.KeyDown('D')) {
            game.cameraX += cos(game.yaw + 1.57f) * speed;
            game.cameraZ += sin(game.yaw + 1.57f) * speed;
        }
        if (engine.KeyDown(VK_SPACE)) {
            game.cameraY += speed;
        }
        if (engine.KeyDown(VK_SHIFT)) {
            game.cameraY -= speed;
        }
    }

    __declspec(dllexport) void GameRender() {
        // Custom rendering would go here
        // The engine handles the basic render loop
    }

    __declspec(dllexport) void GameShutdown() {
        std::cout << "Game shutting down..." << std::endl;
    }
}

bool LoadEngineDLL(const std::wstring& dllPath) {
    HMODULE hDll = LoadLibrary(dllPath.c_str());
    if (!hDll) {
        std::wcerr << L"Failed to load DLL: " << dllPath << std::endl;
        return false;
    }

    // Load engine functions
    engine.Init = (EngineInitFunc)GetProcAddress(hDll, "EngineInit");
    engine.Run = (EngineRunFunc)GetProcAddress(hDll, "EngineRun");
    engine.Shutdown = (EngineShutdownFunc)GetProcAddress(hDll, "EngineShutdown");
    engine.IsRunning = (EngineIsRunningFunc)GetProcAddress(hDll, "EngineIsRunning");
    engine.KeyPressed = (KeyPressedFunc)GetProcAddress(hDll, "KeyPressed");
    engine.KeyDown = (KeyDownFunc)GetProcAddress(hDll, "KeyDown");
    engine.GetMousePos = (GetMousePosFunc)GetProcAddress(hDll, "GetMousePos");
    engine.GetMouseDelta = (GetMouseDeltaFunc)GetProcAddress(hDll, "GetMouseDelta");

    if (!engine.Init || !engine.Run || !engine.Shutdown) {
        std::cerr << "Failed to get required engine functions!" << std::endl;
        FreeLibrary(hDll);
        return false;
    }

    std::cout << "Engine DLL loaded successfully!" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "Project32 Game Launcher" << std::endl;
    std::cout << "======================" << std::endl;

    // Load the engine DLL
    if (!LoadEngineDLL(L"Project32.Core.dll")) {
        std::cerr << "Engine initialization failed!" << std::endl;
        system("pause");
        return 1;
    }

    // Parse command line arguments for window size
    int width = 1920, height = 1080;
    std::string title = "Project32 Game";

    if (argc >= 3) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }
    if (argc >= 4) {
        title = argv[3];
    }

    try {
        // Initialize engine
        if (!engine.Init(width, height, title.c_str())) {
            throw std::runtime_error("Engine initialization failed!");
        }

        std::cout << "Starting game loop..." << std::endl;

        // Run the game
        engine.Run();

        // Cleanup
        engine.Shutdown();
        std::cout << "Game ended successfully." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        engine.Shutdown();
        return 1;
    }

    return 0;
}