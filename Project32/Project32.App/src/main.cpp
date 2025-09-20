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

typedef bool (*EngineInitFunc)(int width, int height, const char* title);
typedef void (*EngineRunFunc)();
typedef void (*EngineShutdownFunc)();
typedef bool (*EngineIsRunningFunc)();

// Input function types
typedef bool (*KeyPressedFunc)(int key);
typedef bool (*KeyDownFunc)(int key);
typedef void (*GetMousePosFunc)(float* x, float* y);
typedef void (*GetMouseDeltaFunc)(float* dx, float* dy);

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