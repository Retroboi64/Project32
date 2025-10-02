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

#include "engine.h"
#include <memory>
#include <thread>
#include <iostream>
#include "UIX.h"
#define UIEXPORT


extern "C" {
    static int g_currentEngineID = -1;

    __declspec(dllexport) bool EngineInit() {
        try {
            g_currentEngineID = EngineManager::Instance()->CreateEngine(1920, 1080, "Project32");
            return g_currentEngineID != -1;
        }
        catch (...) {
            g_currentEngineID = -1;
            return false;
        }
    }

    __declspec(dllexport) void EngineRun() {
        Engine* engine = EngineManager::Instance()->GetEngineByID(g_currentEngineID);
        if (engine) {
            engine->Run();
        }
    }

    __declspec(dllexport) void EngineShutdown() {
        if (g_currentEngineID != -1) {
            EngineManager::Instance()->DestroyEngine(g_currentEngineID);
            g_currentEngineID = -1;
        }
    }

    __declspec(dllexport) bool EngineIsRunning() {
        Engine* engine = EngineManager::Instance()->GetEngineByID(g_currentEngineID);
        return engine && engine->IsRunning();
    }

    __declspec(dllexport) int CreateEngine(int width, int height, const char* title) {
        return EngineManager::Instance()->CreateEngine(width, height, std::string(title));
    }

    __declspec(dllexport) bool DestroyEngine(int engineID) {
        return EngineManager::Instance()->DestroyEngine(engineID);
    }

    __declspec(dllexport) void RunEngine(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (engine) {
            engine->Run();
        }
    }

    __declspec(dllexport) bool IsEngineRunning(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        return engine && engine->IsRunning();
    }

    __declspec(dllexport) int GetEngineCount() {
        return static_cast<int>(EngineManager::Instance()->GetEngineCount());
    }

    __declspec(dllexport) void SetCurrentEngine(int engineID) {
        EngineManager::Instance()->SetCurrentEngine(engineID);
    }

    __declspec(dllexport) int GetCurrentEngineID() {
        return EngineManager::Instance()->GetCurrentEngineID();
    }

    __declspec(dllexport) void DestroyAllEngines() {
        EngineManager::Instance()->DestroyAllEngines();
    }

    __declspec(dllexport) void CleanupEngineManager() {
        EngineManager::DestroyInstance();
    }
}
