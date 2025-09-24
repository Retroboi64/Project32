#include "Project32.API.h"
#include <iostream>

EngineFunctions engine = { 0 };
static HMODULE g_hDllModule = nullptr;

bool LoadEngineDLL(const std::wstring& dllPath) {
    UnloadEngineDLL();

    g_hDllModule = LoadLibraryW(dllPath.c_str());
    if (!g_hDllModule) {
        std::wcerr << L"Failed to load DLL: " << dllPath << L" (Error: " << GetLastError() << L")" << std::endl;
        return false;
    }

    engine.Init = (EngineInitFunc)GetProcAddress(g_hDllModule, "EngineInit");
    engine.Run = (EngineRunFunc)GetProcAddress(g_hDllModule, "EngineRun");
    engine.Shutdown = (EngineShutdownFunc)GetProcAddress(g_hDllModule, "EngineShutdown");
    engine.IsRunning = (EngineIsRunningFunc)GetProcAddress(g_hDllModule, "EngineIsRunning");
    engine.KeyPressed = (KeyPressedFunc)GetProcAddress(g_hDllModule, "KeyPressed");
    engine.KeyDown = (KeyDownFunc)GetProcAddress(g_hDllModule, "KeyDown");
    engine.GetMousePos = (GetMousePosFunc)GetProcAddress(g_hDllModule, "GetMousePos");
    engine.GetMouseDelta = (GetMouseDeltaFunc)GetProcAddress(g_hDllModule, "GetMouseDelta");

    if (!engine.Init || !engine.Run || !engine.Shutdown) {
        std::cerr << "Failed to get required engine functions!" << std::endl;
        UnloadEngineDLL();
        return false;
    }

    std::wcout << L"Engine DLL loaded successfully: " << dllPath << std::endl;
    return true;
}

void UnloadEngineDLL() {
    if (g_hDllModule) {
        if (engine.Shutdown) {
            engine.Shutdown();
        }

        FreeLibrary(g_hDllModule);
        g_hDllModule = nullptr;

        memset(&engine, 0, sizeof(EngineFunctions));

        std::cout << "Engine DLL unloaded." << std::endl;
    }
}

bool IsEngineDLLLoaded() {
    return g_hDllModule != nullptr;
}

#ifdef __cplusplus
namespace P32 {
    HMODULE Engine::hDllModule = nullptr;
    bool Engine::isLoaded = false;

    bool Engine::LoadDLL(const std::wstring& dllPath) {
        bool result = LoadEngineDLL(dllPath);
        hDllModule = g_hDllModule;
        isLoaded = result;
        return result;
    }

    void Engine::UnloadDLL() {
        UnloadEngineDLL();
        hDllModule = nullptr;
        isLoaded = false;
    }

    bool Engine::IsLoaded() {
        return isLoaded && IsEngineDLLLoaded();
    }

    bool Engine::Init(int width, int height, const char* title) {
        if (!isLoaded || !engine.Init) {
            std::cerr << "Engine not loaded or Init function not available!" << std::endl;
            return false;
        }
        return engine.Init(width, height, title);
    }

    void Engine::Run() {
        if (!isLoaded || !engine.Run) {
            std::cerr << "Engine not loaded or Run function not available!" << std::endl;
            return;
        }
        engine.Run();
    }

    void Engine::Shutdown() {
        if (!isLoaded || !engine.Shutdown) {
            std::cerr << "Engine not loaded or Shutdown function not available!" << std::endl;
            return;
        }
        engine.Shutdown();
    }

    bool Engine::IsRunning() {
        if (!isLoaded || !engine.IsRunning) {
            return false;
        }
        return engine.IsRunning();
    }

    bool Engine::KeyPressed(int key) {
        if (!isLoaded || !engine.KeyPressed) {
            return false;
        }
        return engine.KeyPressed(key);
    }

    bool Engine::KeyDown(int key) {
        if (!isLoaded || !engine.KeyDown) {
            return false;
        }
        return engine.KeyDown(key);
    }

    void Engine::GetMousePos(float* x, float* y) {
        if (!isLoaded || !engine.GetMousePos) {
            if (x) *x = 0.0f;
            if (y) *y = 0.0f;
            return;
        }
        engine.GetMousePos(x, y);
    }

    void Engine::GetMouseDelta(float* dx, float* dy) {
        if (!isLoaded || !engine.GetMouseDelta) {
            if (dx) *dx = 0.0f;
            if (dy) *dy = 0.0f;
            return;
        }
        engine.GetMouseDelta(dx, dy);
    }
}
#endif