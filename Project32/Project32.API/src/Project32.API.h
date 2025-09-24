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

    typedef bool (*EngineInitFunc)(int width, int height, const char* title);
    typedef void (*EngineRunFunc)();
    typedef void (*EngineShutdownFunc)();
    typedef bool (*EngineIsRunningFunc)();
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
    };

    extern EngineFunctions engine;

    typedef enum {
        P32_SUCCESS = 0,
        P32_ERROR = -1,
        P32_ERROR_INIT_FAILED = -2,
        P32_ERROR_NOT_INITIALIZED = -3,
        P32_ERROR_INVALID_PARAM = -4,
        P32_ERROR_DLL_LOAD_FAILED = -5,
        P32_ERROR_FUNCTION_NOT_FOUND = -6
    } P32_Result;

    bool LoadEngineDLL(const std::wstring& dllPath);
    void UnloadEngineDLL();
    bool IsEngineDLLLoaded();

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

        static bool Init(int width, int height, const char* title);
        static void Run();
        static void Shutdown();
        static bool IsRunning();
        static bool KeyPressed(int key);
        static bool KeyDown(int key);
        static void GetMousePos(float* x, float* y);
        static void GetMouseDelta(float* dx, float* dy);
    };
}
#endif