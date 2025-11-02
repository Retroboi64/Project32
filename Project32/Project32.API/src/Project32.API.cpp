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

    // Legacy functions
    engine.Init = (EngineInitFunc)GetProcAddress(g_hDllModule, "EngineInit");
    engine.Run = (EngineRunFunc)GetProcAddress(g_hDllModule, "EngineRun");
    engine.Shutdown = (EngineShutdownFunc)GetProcAddress(g_hDllModule, "EngineShutdown");
    engine.IsRunning = (EngineIsRunningFunc)GetProcAddress(g_hDllModule, "EngineIsRunning");
    engine.KeyPressed = (KeyPressedFunc)GetProcAddress(g_hDllModule, "KeyPressed");
    engine.KeyDown = (KeyDownFunc)GetProcAddress(g_hDllModule, "KeyDown");
    engine.GetMousePos = (GetMousePosFunc)GetProcAddress(g_hDllModule, "GetMousePos");
    engine.GetMouseDelta = (GetMouseDeltaFunc)GetProcAddress(g_hDllModule, "GetMouseDelta");

    // Multi-engine management
    engine.CreateEngine = (CreateEngineFunc)GetProcAddress(g_hDllModule, "CreateEngine");
    engine.DestroyEngine = (DestroyEngineFunc)GetProcAddress(g_hDllModule, "DestroyEngine");
    engine.RunEngine = (RunEngineFunc)GetProcAddress(g_hDllModule, "RunEngine");
    engine.RunAllEngines = (RunAllEnginesFunc)GetProcAddress(g_hDllModule, "RunAllEngines");
    engine.IsEngineRunning = (IsEngineRunningFunc)GetProcAddress(g_hDllModule, "IsEngineRunning");
    engine.GetEngineCount = (GetEngineCountFunc)GetProcAddress(g_hDllModule, "GetEngineCount");
    engine.SetCurrentEngine = (SetCurrentEngineFunc)GetProcAddress(g_hDllModule, "SetCurrentEngine");
    engine.GetCurrentEngineID = (GetCurrentEngineIDFunc)GetProcAddress(g_hDllModule, "GetCurrentEngineID");
    engine.DestroyAllEngines = (DestroyAllEnginesFunc)GetProcAddress(g_hDllModule, "DestroyAllEngines");
    engine.CleanupEngineManager = (CleanupEngineManagerFunc)GetProcAddress(g_hDllModule, "CleanupEngineManager");

    // Per-engine input
    engine.EngineKeyPressed = (EngineKeyPressedFunc)GetProcAddress(g_hDllModule, "EngineKeyPressed");
    engine.EngineKeyDown = (EngineKeyDownFunc)GetProcAddress(g_hDllModule, "EngineKeyDown");
    engine.EngineGetMousePos = (EngineGetMousePosFunc)GetProcAddress(g_hDllModule, "EngineGetMousePos");
    engine.EngineGetMouseDelta = (EngineGetMouseDeltaFunc)GetProcAddress(g_hDllModule, "EngineGetMouseDelta");

    // Engine configuration
    engine.SetEngineBackgroundColor = (SetEngineBackgroundColorFunc)GetProcAddress(g_hDllModule, "SetEngineBackgroundColor");
    engine.SetEngineFOV = (SetEngineFOVFunc)GetProcAddress(g_hDllModule, "SetEngineFOV");
    engine.ToggleEngineWireframe = (ToggleEngineWireframeFunc)GetProcAddress(g_hDllModule, "ToggleEngineWireframe");
    engine.ToggleEngineDebugInfo = (ToggleEngineDebugInfoFunc)GetProcAddress(g_hDllModule, "ToggleEngineDebugInfo");
    engine.ToggleEngineRenderScene = (ToggleEngineRenderSceneFunc)GetProcAddress(g_hDllModule, "ToggleEngineRenderScene");

    // Engine window management
    engine.SetEngineWindowSize = (SetEngineWindowSizeFunc)GetProcAddress(g_hDllModule, "SetEngineWindowSize");
    engine.GetEngineWindowSize = (GetEngineWindowSizeFunc)GetProcAddress(g_hDllModule, "GetEngineWindowSize");
    engine.SetEngineWindowTitle = (SetEngineWindowTitleFunc)GetProcAddress(g_hDllModule, "SetEngineWindowTitle");
    engine.SetEngineVSync = (SetEngineVSyncFunc)GetProcAddress(g_hDllModule, "SetEngineVSync");
    engine.GetEngineVSync = (GetEngineVSyncFunc)GetProcAddress(g_hDllModule, "GetEngineVSync");

    // Window creation/management
    engine.CreateEngineWindow = (CreateEngineWindowFunc)GetProcAddress(g_hDllModule, "CreateEngineWindow");
    engine.DestroyEngineWindow = (DestroyEngineWindowFunc)GetProcAddress(g_hDllModule, "DestroyEngineWindow");
    engine.GetEngineWindowCount = (GetEngineWindowCountFunc)GetProcAddress(g_hDllModule, "GetEngineWindowCount");
    engine.SetEngineMainWindow = (SetEngineMainWindowFunc)GetProcAddress(g_hDllModule, "SetEngineMainWindow");
    engine.GetEngineMainWindowID = (GetEngineMainWindowIDFunc)GetProcAddress(g_hDllModule, "GetEngineMainWindowID");
    engine.GetWindowPosition = (GetWindowPositionFunc)GetProcAddress(g_hDllModule, "GetWindowPosition");
    engine.SetWindowPosition = (SetWindowPositionFunc)GetProcAddress(g_hDllModule, "SetWindowPosition");
    engine.IsWindowOpen = (IsWindowOpenFunc)GetProcAddress(g_hDllModule, "IsWindowOpen");

	// Scripting functions/hooks
	engine.LoadScript = (LoadScriptFunc)GetProcAddress(g_hDllModule, "LoadScript");

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

    // Legacy single-engine interface
    bool Engine::Init() {
        if (!isLoaded || !engine.Init) return false;
        return engine.Init();
    }

    void Engine::Run() {
        if (!isLoaded || !engine.Run) return;
        engine.Run();
    }

    void Engine::Shutdown() {
        if (!isLoaded || !engine.Shutdown) return;
        engine.Shutdown();
    }

    bool Engine::IsRunning() {
        if (!isLoaded || !engine.IsRunning) return false;
        return engine.IsRunning();
    }

    bool Engine::KeyPressed(int key) {
        if (!isLoaded || !engine.KeyPressed) return false;
        return engine.KeyPressed(key);
    }

    bool Engine::KeyDown(int key) {
        if (!isLoaded || !engine.KeyDown) return false;
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

    // Multi-engine management
    int Engine::CreateEngine(const std::string& title) {
        if (!isLoaded || !engine.CreateEngine) return -1;
        return engine.CreateEngine(title.c_str());
    }

    bool Engine::DestroyEngine(int engineID) {
        if (!isLoaded || !engine.DestroyEngine) return false;
        return engine.DestroyEngine(engineID);
    }

    void Engine::RunEngine(int engineID) {
        if (!isLoaded || !engine.RunEngine) return;
        engine.RunEngine(engineID);
    }

    void Engine::RunAllEngines() {
        if (!isLoaded || !engine.RunAllEngines) return;
        engine.RunAllEngines();
    }

    bool Engine::IsEngineRunning(int engineID) {
        if (!isLoaded || !engine.IsEngineRunning) return false;
        return engine.IsEngineRunning(engineID);
    }

    int Engine::GetEngineCount() {
        if (!isLoaded || !engine.GetEngineCount) return 0;
        return engine.GetEngineCount();
    }

    void Engine::SetCurrentEngine(int engineID) {
        if (!isLoaded || !engine.SetCurrentEngine) return;
        engine.SetCurrentEngine(engineID);
    }

    int Engine::GetCurrentEngineID() {
        if (!isLoaded || !engine.GetCurrentEngineID) return -1;
        return engine.GetCurrentEngineID();
    }

    void Engine::DestroyAllEngines() {
        if (!isLoaded || !engine.DestroyAllEngines) return;
        engine.DestroyAllEngines();
    }

    void Engine::CleanupEngineManager() {
        if (!isLoaded || !engine.CleanupEngineManager) return;
        engine.CleanupEngineManager();
    }

    // Per-engine input
    bool Engine::EngineKeyPressed(int engineID, int key) {
        if (!isLoaded || !engine.EngineKeyPressed) return false;
        return engine.EngineKeyPressed(engineID, key);
    }

    bool Engine::EngineKeyDown(int engineID, int key) {
        if (!isLoaded || !engine.EngineKeyDown) return false;
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

    // Engine configuration
    void Engine::SetEngineBackgroundColor(int engineID, float r, float g, float b) {
        if (!isLoaded || !engine.SetEngineBackgroundColor) return;
        engine.SetEngineBackgroundColor(engineID, r, g, b);
    }

    void Engine::SetEngineFOV(int engineID, float fov) {
        if (!isLoaded || !engine.SetEngineFOV) return;
        engine.SetEngineFOV(engineID, fov);
    }

    void Engine::ToggleEngineRenderScene(int engineID) {
        if (!isLoaded || !engine.ToggleEngineRenderScene) return;
        engine.ToggleEngineRenderScene(engineID);
    }

    void Engine::ToggleEngineWireframe(int engineID) {
        if (!isLoaded || !engine.ToggleEngineWireframe) return;
        engine.ToggleEngineWireframe(engineID);
    }

    void Engine::ToggleEngineDebugInfo(int engineID) {
        if (!isLoaded || !engine.ToggleEngineDebugInfo) return;
        engine.ToggleEngineDebugInfo(engineID);
    }

    // Engine window management
    bool Engine::SetEngineWindowSize(int engineID, int width, int height) {
        if (!isLoaded || !engine.SetEngineWindowSize) return false;
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
        if (!isLoaded || !engine.SetEngineWindowTitle) return;
        engine.SetEngineWindowTitle(engineID, title.c_str());
    }

    void Engine::SetEngineVSync(int engineID, bool enabled) {
        if (!isLoaded || !engine.SetEngineVSync) return;
        engine.SetEngineVSync(engineID, enabled);
    }

    bool Engine::GetEngineVSync(int engineID) {
        if (!isLoaded || !engine.GetEngineVSync) return false;
        return engine.GetEngineVSync(engineID);
    }

    // Window creation/management
    int Engine::CreateEngineWindow(int engineID, int width, int height, const std::string& title) {
        if (!isLoaded || !engine.CreateEngineWindow) return -1;
        return engine.CreateEngineWindow(engineID, width, height, title.c_str());
    }

    bool Engine::DestroyEngineWindow(int engineID, int windowID) {
        if (!isLoaded || !engine.DestroyEngineWindow) return false;
        return engine.DestroyEngineWindow(engineID, windowID);
    }

    int Engine::GetEngineWindowCount(int engineID) {
        if (!isLoaded || !engine.GetEngineWindowCount) return 0;
        return engine.GetEngineWindowCount(engineID);
    }

    void Engine::SetEngineMainWindow(int engineID, int windowID) {
        if (!isLoaded || !engine.SetEngineMainWindow) return;
        engine.SetEngineMainWindow(engineID, windowID);
    }

    int Engine::GetEngineMainWindowID(int engineID) {
        if (!isLoaded || !engine.GetEngineMainWindowID) return -1;
        return engine.GetEngineMainWindowID(engineID);
    }

    void Engine::GetWindowPosition(int engineID, int windowID, int* x, int* y) {
        if (!isLoaded || !engine.GetWindowPosition) {
            if (x) *x = 0;
            if (y) *y = 0;
            return;
        }
        engine.GetWindowPosition(engineID, windowID, x, y);
    }

    void Engine::SetWindowPosition(int engineID, int windowID, int x, int y) {
        if (!isLoaded || !engine.SetWindowPosition) return;
        engine.SetWindowPosition(engineID, windowID, x, y);
    }

    bool Engine::IsWindowOpen(int engineID, int windowID) {
        if (!isLoaded || !engine.IsWindowOpen) return false;
        return engine.IsWindowOpen(engineID, windowID);
    }

    // WindowInstance implementation
    WindowInstance::WindowInstance(int engineID, int windowID)
        : _engineID(engineID), _windowID(windowID), _valid(true) {
        if (!Engine::IsLoaded() || engineID < 0 || windowID < 0) {
            _valid = false;
        }
    }

    bool WindowInstance::SetSize(int width, int height) {
        if (!_valid) return false;
        return Engine::SetEngineWindowSize(_engineID, width, height);
    }

    void WindowInstance::GetSize(int* width, int* height) const {
        if (!_valid) {
            if (width) *width = 0;
            if (height) *height = 0;
            return;
        }
        Engine::GetEngineWindowSize(_engineID, width, height);
    }

    void WindowInstance::SetPosition(int x, int y) {
        if (_valid) {
            Engine::SetWindowPosition(_engineID, _windowID, x, y);
        }
    }

    void WindowInstance::GetPosition(int* x, int* y) const {
        if (!_valid) {
            if (x) *x = 0;
            if (y) *y = 0;
            return;
        }
        Engine::GetWindowPosition(_engineID, _windowID, x, y);
    }

    void WindowInstance::SetTitle(const std::string& title) {
        if (_valid) {
            Engine::SetEngineWindowTitle(_engineID, title);
        }
    }

    bool WindowInstance::IsOpen() const {
        if (!_valid) return false;
        return Engine::IsWindowOpen(_engineID, _windowID);
    }

    bool WindowInstance::Close() {
        if (!_valid) return false;
        bool result = Engine::DestroyEngineWindow(_engineID, _windowID);
        if (result) _valid = false;
        return result;
    }

    void WindowInstance::SetBackgroundColor(float r, float g, float b) {
        if (_valid) {
            Engine::SetEngineBackgroundColor(_engineID, r, g, b);
        }
    }

    void WindowInstance::SetFOV(float fov) {
        if (_valid) {
            Engine::SetEngineFOV(_engineID, fov);
        }
    }

    void WindowInstance::SetVSync(bool enabled) {
        if (_valid) {
            Engine::SetEngineVSync(_engineID, enabled);
        }
    }

    bool WindowInstance::GetVSync() const {
        if (!_valid) return false;
        return Engine::GetEngineVSync(_engineID);
    }

    void WindowInstance::MakeMain() {
        if (_valid) {
            Engine::SetEngineMainWindow(_engineID, _windowID);
        }
    }

    // EngineInstance implementation
    EngineInstance::EngineInstance(const std::string& title, int width, int height)
        : _engineID(-1), _valid(false) {
        _engineID = Engine::CreateEngine(title);
        _valid = (_engineID != -1);

        if (_valid) {
            int mainWindowID = Engine::GetEngineMainWindowID(_engineID);
            if (mainWindowID != -1) {
                auto mainWindow = std::make_shared<WindowInstance>(_engineID, mainWindowID);
                mainWindow->SetSize(width, height);
                _windows.push_back(mainWindow);
            }
        }
        else {
            std::cerr << "Failed to create engine instance: " << title << std::endl;
        }
    }

    EngineInstance::~EngineInstance() {
        if (_valid) {
            _windows.clear();
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
            _windows.clear();
            Engine::DestroyEngine(_engineID);
            _valid = false;
        }
    }

    bool EngineInstance::IsRunning() const {
        if (!_valid) return false;
        return Engine::IsEngineRunning(_engineID);
    }

    std::shared_ptr<WindowInstance> EngineInstance::CreateNewWindow(int width, int height, const std::string& title) {
        if (!_valid) return nullptr;

        int windowID = Engine::CreateEngineWindow(_engineID, width, height, title);
        if (windowID == -1) return nullptr;

        auto window = std::make_shared<WindowInstance>(_engineID, windowID);
        _windows.push_back(window);
        return window;
    }

    bool EngineInstance::DestroyWindow(int windowID) {
        if (!_valid) return false;

        auto it = std::find_if(_windows.begin(), _windows.end(),
            [windowID](const std::shared_ptr<WindowInstance>& w) {
                return w->GetWindowID() == windowID;
            });

        if (it != _windows.end()) {
            _windows.erase(it);
            return Engine::DestroyEngineWindow(_engineID, windowID);
        }
        return false;
    }

    std::shared_ptr<WindowInstance> EngineInstance::GetWindow(int windowID) const {
        auto it = std::find_if(_windows.begin(), _windows.end(),
            [windowID](const std::shared_ptr<WindowInstance>& w) {
                return w->GetWindowID() == windowID;
            });

        return (it != _windows.end()) ? *it : nullptr;
    }

    std::shared_ptr<WindowInstance> EngineInstance::GetMainWindow() const {
        if (!_valid) return nullptr;

        int mainWindowID = Engine::GetEngineMainWindowID(_engineID);
        return GetWindow(mainWindowID);
    }

    int EngineInstance::GetWindowCount() const {
        if (!_valid) return 0;
        return Engine::GetEngineWindowCount(_engineID);
    }

    std::vector<std::shared_ptr<WindowInstance>> EngineInstance::GetAllWindows() const {
        return _windows;
    }

    void EngineInstance::SetMainWindow(int windowID) {
        if (_valid) {
            Engine::SetEngineMainWindow(_engineID, windowID);
        }
    }

    void EngineInstance::SetMainWindow(std::shared_ptr<WindowInstance> window) {
        if (_valid && window && window->IsValid()) {
            Engine::SetEngineMainWindow(_engineID, window->GetWindowID());
        }
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

    // EngineManager implementation
    EngineManager::~EngineManager() {
        DestroyAllEngines();
    }

    std::shared_ptr<EngineInstance> EngineManager::CreateEngine(const std::string& title, int width, int height) {
        auto instance = std::make_shared<EngineInstance>(title, width, height);
        if (instance->IsValid()) {
            _instances.push_back(instance);
            if (!_currentInstance) {
                _currentInstance = instance;
                instance->MakeCurrent();
            }
            return instance;
        }
        return nullptr;
    }

    bool EngineManager::DestroyEngine(int engineID) {
        auto it = std::find_if(_instances.begin(), _instances.end(),
            [engineID](const std::shared_ptr<EngineInstance>& inst) {
                return inst->GetID() == engineID;
            });

        if (it != _instances.end()) {
            if (_currentInstance == *it) {
                _currentInstance = nullptr;
                if (!_instances.empty() && _instances.size() > 1) {
                    _currentInstance = (_instances[0] == *it) ? _instances[1] : _instances[0];
                    _currentInstance->MakeCurrent();
                }
            }
            _instances.erase(it);
            return true;
        }
        return false;
    }

    void EngineManager::DestroyAllEngines() {
        _currentInstance = nullptr;
        _instances.clear();
        Engine::DestroyAllEngines();
    }

    std::shared_ptr<EngineInstance> EngineManager::GetEngine(int engineID) const {
        auto it = std::find_if(_instances.begin(), _instances.end(),
            [engineID](const std::shared_ptr<EngineInstance>& inst) {
                return inst->GetID() == engineID;
            });

        return (it != _instances.end()) ? *it : nullptr;
    }

    void EngineManager::SetCurrentEngine(int engineID) {
        auto engine = GetEngine(engineID);
        if (engine) {
            _currentInstance = engine;
            engine->MakeCurrent();
        }
    }

    void EngineManager::SetCurrentEngine(std::shared_ptr<EngineInstance> instance) {
        if (instance && instance->IsValid()) {
            _currentInstance = instance;
            instance->MakeCurrent();
        }
    }

    std::vector<std::shared_ptr<EngineInstance>> EngineManager::GetAllEngines() const {
        return _instances;
    }
}
#endif