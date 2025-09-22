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

static std::unique_ptr<Engine> g_engine;
std::unique_ptr<Window> window = nullptr;

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
