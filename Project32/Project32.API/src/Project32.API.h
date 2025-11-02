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

#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#ifdef PROJECT32_API_EXPORTS
#define P32_API __declspec(dllexport)
#else
#define P32_API __declspec(dllimport)
#endif
#define P32_CALL __cdecl
#else
#define P32_API
#define P32_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// Main engine functions
    typedef bool (*EngineInitFunc)();
    typedef void (*EngineRunFunc)();
    typedef void (*EngineShutdownFunc)();
    typedef bool (*EngineIsRunningFunc)();
    typedef bool (*KeyPressedFunc)(int key);
    typedef bool (*KeyDownFunc)(int key);
    typedef void (*GetMousePosFunc)(float* x, float* y);
    typedef void (*GetMouseDeltaFunc)(float* dx, float* dy);

	// Multi-engine management
    typedef int (*CreateEngineFunc)(const char* title);
    typedef bool (*DestroyEngineFunc)(int engineID);
    typedef void (*RunEngineFunc)(int engineID);
    typedef void (*RunAllEnginesFunc)();
    typedef bool (*IsEngineRunningFunc)(int engineID);
    typedef int (*GetEngineCountFunc)();
    typedef void (*SetCurrentEngineFunc)(int engineID);
    typedef int (*GetCurrentEngineIDFunc)();
    typedef void (*DestroyAllEnginesFunc)();
    typedef void (*CleanupEngineManagerFunc)();

	// Per-engine input
    typedef bool (*EngineKeyPressedFunc)(int engineID, int key);
    typedef bool (*EngineKeyDownFunc)(int engineID, int key);
    typedef void (*EngineGetMousePosFunc)(int engineID, float* x, float* y);
    typedef void (*EngineGetMouseDeltaFunc)(int engineID, float* dx, float* dy);

	// Engine configuration
    typedef void (*SetEngineBackgroundColorFunc)(int engineID, float r, float g, float b);
    typedef void (*SetEngineFOVFunc)(int engineID, float fov);
    typedef void (*ToggleEngineRenderSceneFunc)(int engineID);
    typedef void (*ToggleEngineWireframeFunc)(int engineID);
    typedef void (*ToggleEngineDebugInfoFunc)(int engineID);

	// Engine window management
    typedef bool (*SetEngineWindowSizeFunc)(int engineID, int width, int height);
    typedef void (*GetEngineWindowSizeFunc)(int engineID, int* width, int* height);
    typedef void (*SetEngineWindowTitleFunc)(int engineID, const char* title);
    typedef void (*SetEngineVSyncFunc)(int engineID, bool enabled);
    typedef bool (*GetEngineVSyncFunc)(int engineID);

	// Window creation/management
    typedef int (*CreateEngineWindowFunc)(int engineID, int width, int height, const char* title);
    typedef bool (*DestroyEngineWindowFunc)(int engineID, int windowID);
    typedef int (*GetEngineWindowCountFunc)(int engineID);
    typedef void (*SetEngineMainWindowFunc)(int engineID, int windowID);
    typedef int (*GetEngineMainWindowIDFunc)(int engineID);
    typedef void (*GetWindowPositionFunc)(int engineID, int windowID, int* x, int* y);
    typedef void (*SetWindowPositionFunc)(int engineID, int windowID, int x, int y);
    typedef bool (*IsWindowOpenFunc)(int engineID, int windowID);

	// Scripting functions/hooks
	typedef void (*LoadScriptFunc)(int engineID, const char* scriptPath);

    struct EngineFunctions {
		// Main engine functions
        EngineInitFunc Init;
        EngineRunFunc Run;
        EngineShutdownFunc Shutdown;
        EngineIsRunningFunc IsRunning;
        KeyPressedFunc KeyPressed;
        KeyDownFunc KeyDown;
        GetMousePosFunc GetMousePos;
        GetMouseDeltaFunc GetMouseDelta;

		// Multi-engine management
        CreateEngineFunc CreateEngine;
        DestroyEngineFunc DestroyEngine;
        RunEngineFunc RunEngine;
        RunAllEnginesFunc RunAllEngines;
        IsEngineRunningFunc IsEngineRunning;
        GetEngineCountFunc GetEngineCount;
        SetCurrentEngineFunc SetCurrentEngine;
        GetCurrentEngineIDFunc GetCurrentEngineID;
        DestroyAllEnginesFunc DestroyAllEngines;
        CleanupEngineManagerFunc CleanupEngineManager;

		// Per-engine input
        EngineKeyPressedFunc EngineKeyPressed;
        EngineKeyDownFunc EngineKeyDown;
        EngineGetMousePosFunc EngineGetMousePos;
        EngineGetMouseDeltaFunc EngineGetMouseDelta;

		// Engine configuration
        SetEngineBackgroundColorFunc SetEngineBackgroundColor;
        SetEngineFOVFunc SetEngineFOV;
        ToggleEngineRenderSceneFunc ToggleEngineRenderScene;
        ToggleEngineWireframeFunc ToggleEngineWireframe;
        ToggleEngineDebugInfoFunc ToggleEngineDebugInfo;

		// Engine window management
        SetEngineWindowSizeFunc SetEngineWindowSize;
        GetEngineWindowSizeFunc GetEngineWindowSize;
        SetEngineWindowTitleFunc SetEngineWindowTitle;
        SetEngineVSyncFunc SetEngineVSync;
        GetEngineVSyncFunc GetEngineVSync;

		// Window creation/management
        CreateEngineWindowFunc CreateEngineWindow;
        DestroyEngineWindowFunc DestroyEngineWindow;
        GetEngineWindowCountFunc GetEngineWindowCount;
        SetEngineMainWindowFunc SetEngineMainWindow;
        GetEngineMainWindowIDFunc GetEngineMainWindowID;
        GetWindowPositionFunc GetWindowPosition;
        SetWindowPositionFunc SetWindowPosition;
        IsWindowOpenFunc IsWindowOpen;

		// Scripting functions/hooks
		LoadScriptFunc LoadScript;
    };

    extern EngineFunctions engine;

    typedef enum {
        P32_SUCCESS = 0,
        P32_ERROR = -1,
        P32_ERROR_INIT_FAILED = -2,
        P32_ERROR_NOT_INITIALIZED = -3,
        P32_ERROR_INVALID_PARAM = -4,
        P32_ERROR_DLL_LOAD_FAILED = -5,
        P32_ERROR_FUNCTION_NOT_FOUND = -6,
        P32_ERROR_ENGINE_NOT_FOUND = -7,
        P32_ERROR_INVALID_ENGINE_ID = -8,
        P32_ERROR_WINDOW_NOT_FOUND = -9,
        P32_ERROR_INVALID_WINDOW_ID = -10
    } P32_Result;

    P32_API bool P32_CALL LoadEngineDLL(const std::wstring& dllPath);
    P32_API void P32_CALL UnloadEngineDLL();
    P32_API bool P32_CALL IsEngineDLLLoaded();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace P32 {
    class EngineInstance;
    class WindowInstance;

    class Engine {
    private:
        static HMODULE hDllModule;
        static bool isLoaded;

    public:
        static bool LoadDLL(const std::wstring& dllPath);
        static void UnloadDLL();
        static bool IsLoaded();

        static bool Init();
        static void Run();
        static void Shutdown();
        static bool IsRunning();
        static bool KeyPressed(int key);
        static bool KeyDown(int key);
        static void GetMousePos(float* x, float* y);
        static void GetMouseDelta(float* dx, float* dy);

        static int CreateEngine(const std::string& title);
        static bool DestroyEngine(int engineID);
        static void RunEngine(int engineID);
        static void RunAllEngines();
        static bool IsEngineRunning(int engineID);
        static int GetEngineCount();
        static void SetCurrentEngine(int engineID);
        static int GetCurrentEngineID();
        static void DestroyAllEngines();
        static void CleanupEngineManager();

        static bool EngineKeyPressed(int engineID, int key);
        static bool EngineKeyDown(int engineID, int key);
        static void EngineGetMousePos(int engineID, float* x, float* y);
        static void EngineGetMouseDelta(int engineID, float* dx, float* dy);

        static void SetEngineBackgroundColor(int engineID, float r, float g, float b);
        static void SetEngineFOV(int engineID, float fov);
        static void ToggleEngineRenderScene(int engineID);
        static void ToggleEngineWireframe(int engineID);
        static void ToggleEngineDebugInfo(int engineID);

        static bool SetEngineWindowSize(int engineID, int width, int height);
        static void GetEngineWindowSize(int engineID, int* width, int* height);
        static void SetEngineWindowTitle(int engineID, const std::string& title);
        static void SetEngineVSync(int engineID, bool enabled);
        static bool GetEngineVSync(int engineID);

        static int CreateEngineWindow(int engineID, int width, int height, const std::string& title);
        static bool DestroyEngineWindow(int engineID, int windowID);
        static int GetEngineWindowCount(int engineID);
        static void SetEngineMainWindow(int engineID, int windowID);
        static int GetEngineMainWindowID(int engineID);
        static void GetWindowPosition(int engineID, int windowID, int* x, int* y);
        static void SetWindowPosition(int engineID, int windowID, int x, int y);
        static bool IsWindowOpen(int engineID, int windowID);
    };

    class WindowInstance {
    private:
        int _engineID;
        int _windowID;
        bool _valid;

    public:
        WindowInstance(int engineID, int windowID);
        ~WindowInstance() = default;

        bool IsValid() const { return _valid; }
        int GetWindowID() const { return _windowID; }
        int GetEngineID() const { return _engineID; }

        bool SetSize(int width, int height);
        void GetSize(int* width, int* height) const;
        void SetPosition(int x, int y);
        void GetPosition(int* x, int* y) const;
        void SetTitle(const std::string& title);
        bool IsOpen() const;
        bool Close();

        void SetBackgroundColor(float r, float g, float b);
        void SetFOV(float fov);
        void SetVSync(bool enabled);
        bool GetVSync() const;

        void MakeMain();
    };

    class EngineInstance {
    private:
        int _engineID;
        bool _valid;
        std::vector<std::shared_ptr<WindowInstance>> _windows;

    public:
        EngineInstance(const std::string& title, int width = 800, int height = 600);
        ~EngineInstance();

        void Run();
        void Shutdown();
        bool IsRunning() const;
        int GetID() const { return _engineID; }
        bool IsValid() const { return _valid; }

        std::shared_ptr<WindowInstance> CreateNewWindow(int width, int height, const std::string& title);
        bool DestroyWindow(int windowID);
        std::shared_ptr<WindowInstance> GetWindow(int windowID) const;
        std::shared_ptr<WindowInstance> GetMainWindow() const;
        int GetWindowCount() const;
        std::vector<std::shared_ptr<WindowInstance>> GetAllWindows() const;
        void SetMainWindow(int windowID);
        void SetMainWindow(std::shared_ptr<WindowInstance> window);

        bool KeyPressed(int key) const;
        bool KeyDown(int key) const;
        void GetMousePos(float* x, float* y) const;
        void GetMouseDelta(float* dx, float* dy) const;

        void SetBackgroundColor(float r, float g, float b);
        void SetFOV(float fov);
        void ToggleWireframe();
        void ToggleDebugInfo();
        bool SetWindowSize(int width, int height);
        void GetWindowSize(int* width, int* height) const;
        void SetWindowTitle(const std::string& title);
        void SetVSync(bool enabled);
        bool GetVSync() const;

        void MakeCurrent();
    };

    class EngineManager {
    private:
        std::vector<std::shared_ptr<EngineInstance>> _instances;
        std::shared_ptr<EngineInstance> _currentInstance;

    public:
        EngineManager() = default;
        ~EngineManager();

        std::shared_ptr<EngineInstance> CreateEngine(const std::string& title, int width = 800, int height = 600);
        bool DestroyEngine(int engineID);
        void DestroyAllEngines();

        std::shared_ptr<EngineInstance> GetEngine(int engineID) const;
        std::shared_ptr<EngineInstance> GetCurrentEngine() const { return _currentInstance; }
        void SetCurrentEngine(int engineID);
        void SetCurrentEngine(std::shared_ptr<EngineInstance> instance);

        int GetEngineCount() const { return static_cast<int>(_instances.size()); }
        std::vector<std::shared_ptr<EngineInstance>> GetAllEngines() const;
    };
}
#endif