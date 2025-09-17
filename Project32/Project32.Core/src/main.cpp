#include "engine.h"
#include <memory>

static std::unique_ptr<Engine> g_engine;

extern "C" {
    __declspec(dllexport) bool EngineInit() {
        try {
            g_engine = std::make_unique<Engine>(1920, 1080, "Project32");
            return true;
        }
        catch (...) {
            g_engine.reset();
            return false;
        }
    }

    __declspec(dllexport) void EngineRun() {
        if (g_engine) {
            g_engine->Run();
        }
    }

    __declspec(dllexport) void EngineShutdown() {
        if (g_engine) {
            g_engine->Shutdown();
            g_engine.reset();
        }
    }
} 
