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

#include "Project32.API.h"
#include <iostream>
#include <vector>

EngineFunctions engine = { 0 };
static HMODULE g_hDllModule = nullptr;

bool LoadEngineDLL(const std::wstring& dllPath) {
    UnloadEngineDLL();

    DWORD fileAttr = GetFileAttributesW(dllPath.c_str());
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << L"DLL file not found: " << dllPath << std::endl;
        return false;
    }

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

    engine.CreateEngine = (CreateEngineFunc)GetProcAddress(g_hDllModule, "CreateEngine");
    engine.DestroyEngine = (DestroyEngineFunc)GetProcAddress(g_hDllModule, "DestroyEngine");
    engine.RunEngine = (RunEngineFunc)GetProcAddress(g_hDllModule, "RunEngine");
    engine.IsEngineRunning = (IsEngineRunningFunc)GetProcAddress(g_hDllModule, "IsEngineRunning");
    engine.GetEngineCount = (GetEngineCountFunc)GetProcAddress(g_hDllModule, "GetEngineCount");
    engine.SetCurrentEngine = (SetCurrentEngineFunc)GetProcAddress(g_hDllModule, "SetCurrentEngine");
    engine.GetCurrentEngineID = (GetCurrentEngineIDFunc)GetProcAddress(g_hDllModule, "GetCurrentEngineID");
    engine.DestroyAllEngines = (DestroyAllEnginesFunc)GetProcAddress(g_hDllModule, "DestroyAllEngines");
    engine.CleanupEngineManager = (CleanupEngineManagerFunc)GetProcAddress(g_hDllModule, "CleanupEngineManager");

    engine.EngineKeyPressed = (EngineKeyPressedFunc)GetProcAddress(g_hDllModule, "EngineKeyPressed");
    engine.EngineKeyDown = (EngineKeyDownFunc)GetProcAddress(g_hDllModule, "EngineKeyDown");
    engine.EngineGetMousePos = (EngineGetMousePosFunc)GetProcAddress(g_hDllModule, "EngineGetMousePos");
    engine.EngineGetMouseDelta = (EngineGetMouseDeltaFunc)GetProcAddress(g_hDllModule, "EngineGetMouseDelta");

    engine.SetEngineBackgroundColor = (SetEngineBackgroundColorFunc)GetProcAddress(g_hDllModule, "SetEngineBackgroundColor");
    engine.SetEngineFOV = (SetEngineFOVFunc)GetProcAddress(g_hDllModule, "SetEngineFOV");
    engine.ToggleEngineWireframe = (ToggleEngineWireframeFunc)GetProcAddress(g_hDllModule, "ToggleEngineWireframe");
    engine.ToggleEngineDebugInfo = (ToggleEngineDebugInfoFunc)GetProcAddress(g_hDllModule, "ToggleEngineDebugInfo");
    engine.ToggleEngineRenderScene = (ToggleEngineRenderSceneFunc)GetProcAddress(g_hDllModule, "ToggleEngineRenderScene");

    engine.SetEngineWindowSize = (SetEngineWindowSizeFunc)GetProcAddress(g_hDllModule, "SetEngineWindowSize");
    engine.GetEngineWindowSize = (GetEngineWindowSizeFunc)GetProcAddress(g_hDllModule, "GetEngineWindowSize");
    engine.SetEngineWindowTitle = (SetEngineWindowTitleFunc)GetProcAddress(g_hDllModule, "SetEngineWindowTitle");
    engine.SetEngineVSync = (SetEngineVSyncFunc)GetProcAddress(g_hDllModule, "SetEngineVSync");
    engine.GetEngineVSync = (GetEngineVSyncFunc)GetProcAddress(g_hDllModule, "GetEngineVSync");

    if (!engine.Init || !engine.Run || !engine.Shutdown) {
        std::cerr << "Warning: Legacy engine functions not found. Multi-engine mode only." << std::endl;
    }

    if (!engine.CreateEngine || !engine.DestroyEngine || !engine.RunEngine) {
        std::cerr << "Warning: Multi-engine functions not found. Legacy mode only." << std::endl;
    }

    std::wcout << L"Engine DLL loaded successfully: " << dllPath << std::endl;
    return true;
}

void UnloadEngineDLL() {
    if (g_hDllModule) {
        if (engine.CleanupEngineManager) {
            engine.CleanupEngineManager();
        }
        else if (engine.Shutdown) {
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

    bool Engine::Init() {
        if (!isLoaded || !engine.Init) {
            std::cerr << "Engine not loaded or Init function not available!" << std::endl;
            return false;
        }
        return engine.Init();
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

    int Engine::CreateEngine(int width, int height, const std::string& title) {
        if (!isLoaded || !engine.CreateEngine) {
            std::cerr << "Engine not loaded or CreateEngine function not available!" << std::endl;
            return -1;
        }
        return engine.CreateEngine(width, height, title.c_str());
    }

    bool Engine::DestroyEngine(int engineID) {
        if (!isLoaded || !engine.DestroyEngine) {
            std::cerr << "Engine not loaded or DestroyEngine function not available!" << std::endl;
            return false;
        }
        return engine.DestroyEngine(engineID);
    }

    void Engine::RunEngine(int engineID) {
        if (!isLoaded || !engine.RunEngine) {
            std::cerr << "Engine not loaded or RunEngine function not available!" << std::endl;
            return;
        }
        engine.RunEngine(engineID);
    }

    bool Engine::IsEngineRunning(int engineID) {
        if (!isLoaded || !engine.IsEngineRunning) {
            return false;
        }
        return engine.IsEngineRunning(engineID);
    }

    int Engine::GetEngineCount() {
        if (!isLoaded || !engine.GetEngineCount) {
            return 0;
        }
        return engine.GetEngineCount();
    }

    void Engine::SetCurrentEngine(int engineID) {
        if (!isLoaded || !engine.SetCurrentEngine) {
            std::cerr << "Engine not loaded or SetCurrentEngine function not available!" << std::endl;
            return;
        }
        engine.SetCurrentEngine(engineID);
    }

    int Engine::GetCurrentEngineID() {
        if (!isLoaded || !engine.GetCurrentEngineID) {
            return -1;
        }
        return engine.GetCurrentEngineID();
    }

    void Engine::DestroyAllEngines() {
        if (!isLoaded || !engine.DestroyAllEngines) {
            std::cerr << "Engine not loaded or DestroyAllEngines function not available!" << std::endl;
            return;
        }
        engine.DestroyAllEngines();
    }

    void Engine::CleanupEngineManager() {
        if (!isLoaded || !engine.CleanupEngineManager) {
            std::cerr << "Engine not loaded or CleanupEngineManager function not available!" << std::endl;
            return;
        }
        engine.CleanupEngineManager();
    }

    bool Engine::EngineKeyPressed(int engineID, int key) {
        if (!isLoaded || !engine.EngineKeyPressed) {
            return false;
        }
        return engine.EngineKeyPressed(engineID, key);
    }

    bool Engine::EngineKeyDown(int engineID, int key) {
        if (!isLoaded || !engine.EngineKeyDown) {
            return false;
        }
        return engine.EngineKeyDown(engineID, key);
    }

    void Engine::EngineGetMousePos(int engineID, float* x, float* y) {
        if (!isLoaded || !engine.EngineGetMousePos) {
            if (x) *x = 0.0f;
            if (y) *y = 0.0f;
            return;
        }
        engine.EngineGetMousePos(engineID, x, y);
    }

    void Engine::EngineGetMouseDelta(int engineID, float* dx, float* dy) {
        if (!isLoaded || !engine.EngineGetMouseDelta) {
            if (dx) *dx = 0.0f;
            if (dy) *dy = 0.0f;
            return;
        }
        engine.EngineGetMouseDelta(engineID, dx, dy);
    }

    void Engine::SetEngineBackgroundColor(int engineID, float r, float g, float b) {
        if (!isLoaded || !engine.SetEngineBackgroundColor) {
            std::cerr << "Engine not loaded or SetEngineBackgroundColor function not available!" << std::endl;
            return;
        }
        engine.SetEngineBackgroundColor(engineID, r, g, b);
    }

    void Engine::SetEngineFOV(int engineID, float fov) {
        if (!isLoaded || !engine.SetEngineFOV) {
            std::cerr << "Engine not loaded or SetEngineFOV function not available!" << std::endl;
            return;
        }
        engine.SetEngineFOV(engineID, fov);
    }

    void Engine::ToggleEngineRenderScene(int engineID) {
        if (!isLoaded || !engine.ToggleEngineRenderScene) {
            std::cerr << "Engine not loaded or ToggleEngineRenderScene function not available!" << std::endl;
            return;
        }
        engine.ToggleEngineRenderScene(engineID);
    }

    void Engine::ToggleEngineWireframe(int engineID) {
        if (!isLoaded || !engine.ToggleEngineWireframe) {
            std::cerr << "Engine not loaded or ToggleEngineWireframe function not available!" << std::endl;
            return;
        }
        engine.ToggleEngineWireframe(engineID);
    }

    void Engine::ToggleEngineDebugInfo(int engineID) {
        if (!isLoaded || !engine.ToggleEngineDebugInfo) {
            std::cerr << "Engine not loaded or ToggleEngineDebugInfo function not available!" << std::endl;
            return;
        }
        engine.ToggleEngineDebugInfo(engineID);
    }

    bool Engine::SetEngineWindowSize(int engineID, int width, int height) {
        if (!isLoaded || !engine.SetEngineWindowSize) {
            std::cerr << "Engine not loaded or SetEngineWindowSize function not available!" << std::endl;
            return false;
        }
        return engine.SetEngineWindowSize(engineID, width, height);
    }

    void Engine::GetEngineWindowSize(int engineID, int* width, int* height) {
        if (!isLoaded || !engine.GetEngineWindowSize) {
            if (width) *width = 0;
            if (height) *height = 0;
            return;
        }
        engine.GetEngineWindowSize(engineID, width, height);
    }

    void Engine::SetEngineWindowTitle(int engineID, const std::string& title) {
        if (!isLoaded || !engine.SetEngineWindowTitle) {
            std::cerr << "Engine not loaded or SetEngineWindowTitle function not available!" << std::endl;
            return;
        }
        engine.SetEngineWindowTitle(engineID, title.c_str());
    }

    void Engine::SetEngineVSync(int engineID, bool enabled) {
        if (!isLoaded || !engine.SetEngineVSync) {
            std::cerr << "Engine not loaded or SetEngineVSync function not available!" << std::endl;
            return;
        }
        engine.SetEngineVSync(engineID, enabled);
    }

    bool Engine::GetEngineVSync(int engineID) {
        if (!isLoaded || !engine.GetEngineVSync) {
            return false;
        }
        return engine.GetEngineVSync(engineID);
    }

    // EngineInstance implementation
    EngineInstance::EngineInstance(int width, int height, const std::string& title)
        : _engineID(-1), _valid(false) {
        _engineID = Engine::CreateEngine(width, height, title);
        _valid = (_engineID != -1);
        if (!_valid) {
            std::cerr << "Failed to create engine instance" << std::endl;
        }
    }

    EngineInstance::~EngineInstance() {
        if (_valid) {
            Engine::DestroyEngine(_engineID);
        }
    }

    void EngineInstance::Run() {
        if (_valid) {
            Engine::RunEngine(_engineID);
        }
    }

    void EngineInstance::Shutdown() {
        if (_valid) {
            Engine::DestroyEngine(_engineID);
            _valid = false;
        }
    }

    bool EngineInstance::IsRunning() const {
        if (!_valid) return false;
        return Engine::IsEngineRunning(_engineID);
    }

    bool EngineInstance::KeyPressed(int key) const {
        if (!_valid) return false;
        return Engine::EngineKeyPressed(_engineID, key);
    }

    bool EngineInstance::KeyDown(int key) const {
        if (!_valid) return false;
        return Engine::EngineKeyDown(_engineID, key);
    }

    void EngineInstance::GetMousePos(float* x, float* y) const {
        if (!_valid) {
            if (x) *x = 0.0f;
            if (y) *y = 0.0f;
            return;
        }
        Engine::EngineGetMousePos(_engineID, x, y);
    }

    void EngineInstance::GetMouseDelta(float* dx, float* dy) const {
        if (!_valid) {
            if (dx) *dx = 0.0f;
            if (dy) *dy = 0.0f;
            return;
        }
        Engine::EngineGetMouseDelta(_engineID, dx, dy);
    }

    void EngineInstance::SetBackgroundColor(float r, float g, float b) {
        if (_valid) {
            Engine::SetEngineBackgroundColor(_engineID, r, g, b);
        }
    }

    void EngineInstance::SetFOV(float fov) {
        if (_valid) {
            Engine::SetEngineFOV(_engineID, fov);
        }
    }

    void EngineInstance::ToggleWireframe() {
        if (_valid) {
            Engine::ToggleEngineWireframe(_engineID);
        }
    }

    void EngineInstance::ToggleDebugInfo() {
        if (_valid) {
            Engine::ToggleEngineDebugInfo(_engineID);
        }
    }

    bool EngineInstance::SetWindowSize(int width, int height) {
        if (!_valid) return false;
        return Engine::SetEngineWindowSize(_engineID, width, height);
    }

    void EngineInstance::GetWindowSize(int* width, int* height) const {
        if (!_valid) {
            if (width) *width = 0;
            if (height) *height = 0;
            return;
        }
        Engine::GetEngineWindowSize(_engineID, width, height);
    }

    void EngineInstance::SetWindowTitle(const std::string& title) {
        if (_valid) {
            Engine::SetEngineWindowTitle(_engineID, title);
        }
    }

    void EngineInstance::SetVSync(bool enabled) {
        if (_valid) {
            Engine::SetEngineVSync(_engineID, enabled);
        }
    }

    bool EngineInstance::GetVSync() const {
        if (!_valid) return false;
        return Engine::GetEngineVSync(_engineID);
    }

    void EngineInstance::MakeCurrent() {
        if (_valid) {
            Engine::SetCurrentEngine(_engineID);
        }
    }
}
#endif