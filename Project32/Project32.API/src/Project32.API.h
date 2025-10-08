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

    typedef bool (*EngineInitFunc)();
    typedef void (*EngineRunFunc)();
    typedef void (*EngineShutdownFunc)();
    typedef bool (*EngineIsRunningFunc)();
    typedef bool (*KeyPressedFunc)(int key);
    typedef bool (*KeyDownFunc)(int key);
    typedef void (*GetMousePosFunc)(float* x, float* y);
    typedef void (*GetMouseDeltaFunc)(float* dx, float* dy);

    typedef int (*CreateEngineFunc)(const char* title);
    typedef bool (*DestroyEngineFunc)(int engineID);
    typedef void (*RunEngineFunc)(int engineID);
    typedef bool (*IsEngineRunningFunc)(int engineID);
    typedef int (*GetEngineCountFunc)();
    typedef void (*SetCurrentEngineFunc)(int engineID);
    typedef int (*GetCurrentEngineIDFunc)();
    typedef void (*DestroyAllEnginesFunc)();
    typedef void (*CleanupEngineManagerFunc)();

    typedef bool (*EngineKeyPressedFunc)(int engineID, int key);
    typedef bool (*EngineKeyDownFunc)(int engineID, int key);
    typedef void (*EngineGetMousePosFunc)(int engineID, float* x, float* y);
    typedef void (*EngineGetMouseDeltaFunc)(int engineID, float* dx, float* dy);

    typedef void (*SetEngineBackgroundColorFunc)(int engineID, float r, float g, float b);
    typedef void (*SetEngineFOVFunc)(int engineID, float fov);
    typedef void (*ToggleEngineRenderSceneFunc)(int engineID);
    typedef void (*ToggleEngineWireframeFunc)(int engineID);
    typedef void (*ToggleEngineDebugInfoFunc)(int engineID);

    typedef bool (*SetEngineWindowSizeFunc)(int engineID, int width, int height);
    typedef void (*GetEngineWindowSizeFunc)(int engineID, int* width, int* height);
    typedef void (*SetEngineWindowTitleFunc)(int engineID, const char* title);
    typedef void (*SetEngineVSyncFunc)(int engineID, bool enabled);
    typedef bool (*GetEngineVSyncFunc)(int engineID);

    struct EngineFunctions {
        EngineInitFunc Init;
        EngineRunFunc Run;
        EngineShutdownFunc Shutdown;
        EngineIsRunningFunc IsRunning;
        KeyPressedFunc KeyPressed;
        KeyDownFunc KeyDown;
        GetMousePosFunc GetMousePos;
        GetMouseDeltaFunc GetMouseDelta;

        CreateEngineFunc CreateEngine;
        DestroyEngineFunc DestroyEngine;
        RunEngineFunc RunEngine;
        IsEngineRunningFunc IsEngineRunning;
        GetEngineCountFunc GetEngineCount;
        SetCurrentEngineFunc SetCurrentEngine;
        GetCurrentEngineIDFunc GetCurrentEngineID;
        DestroyAllEnginesFunc DestroyAllEngines;
        CleanupEngineManagerFunc CleanupEngineManager;

        EngineKeyPressedFunc EngineKeyPressed;
        EngineKeyDownFunc EngineKeyDown;
        EngineGetMousePosFunc EngineGetMousePos;
        EngineGetMouseDeltaFunc EngineGetMouseDelta;

        SetEngineBackgroundColorFunc SetEngineBackgroundColor;
        SetEngineFOVFunc SetEngineFOV;
        ToggleEngineRenderSceneFunc ToggleEngineRenderScene;
        ToggleEngineWireframeFunc ToggleEngineWireframe;
        ToggleEngineDebugInfoFunc ToggleEngineDebugInfo;

        SetEngineWindowSizeFunc SetEngineWindowSize;
        GetEngineWindowSizeFunc GetEngineWindowSize;
        SetEngineWindowTitleFunc SetEngineWindowTitle;
        SetEngineVSyncFunc SetEngineVSync;
        GetEngineVSyncFunc GetEngineVSync;
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
        P32_ERROR_INVALID_ENGINE_ID = -8
    } P32_Result;

    P32_API bool P32_CALL LoadEngineDLL(const std::wstring& dllPath);
    P32_API void P32_CALL UnloadEngineDLL();
    P32_API bool P32_CALL IsEngineDLLLoaded();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace P32 {
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
    };

    class EngineInstance {
    private:
        int _engineID;
        bool _valid;

    public:
        EngineInstance(const std::string& title);
        ~EngineInstance();

        void Run();
        void Shutdown();
        bool IsRunning() const;
        int GetID() const { return _engineID; }
        bool IsValid() const { return _valid; }

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
}
#endif