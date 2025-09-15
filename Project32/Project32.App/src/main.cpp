#include <windows.h>
#include <iostream>

typedef bool (*EngineInitFunc)();
typedef void (*EngineUpdateFunc)();
typedef void (*EngineShutdownFunc)();
typedef const char* (*EngineGetStatusFunc)();

int main() {
    HMODULE hDll = LoadLibrary(L"Project32.Core.dll");
    if (!hDll) {
        std::cerr << "Failed to load DLL!" << std::endl;
        return 1;
    }

    EngineInitFunc EngineInit = (EngineInitFunc)GetProcAddress(hDll, "EngineInit");
    EngineUpdateFunc EngineUpdate = (EngineUpdateFunc)GetProcAddress(hDll, "EngineUpdate");
    EngineShutdownFunc EngineShutdown = (EngineShutdownFunc)GetProcAddress(hDll, "EngineShutdown");
    EngineGetStatusFunc EngineGetStatus = (EngineGetStatusFunc)GetProcAddress(hDll, "EngineGetStatus");

    if (!EngineInit || !EngineUpdate || !EngineShutdown || !EngineGetStatus) {
        std::cerr << "Failed to get function address!" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    EngineInit();
    std::cout << "Status after init: " << EngineGetStatus() << std::endl;

    EngineUpdate();
    std::cout << "Status after update: " << EngineGetStatus() << std::endl;

    EngineShutdown();
    std::cout << "Status after shutdown: " << EngineGetStatus() << std::endl;

    FreeLibrary(hDll);
    return 0;
}
