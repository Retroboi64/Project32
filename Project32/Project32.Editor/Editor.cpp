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

#include "Editor.h"
#include "../Project32.API/src/Project32.API.h"
#include <iostream>
#include <unordered_map>
#include <filesystem>

static bool editor_initialized = false;
static bool engine_ready = false;

std::wstring GetDLLPath() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    auto exeDir = std::filesystem::path(exePath).parent_path();
    auto dllPath = exeDir / L"Project32.Core.dll";

    return dllPath.wstring();
}

extern "C" {
    void InitializeEditor() {
        if (editor_initialized) return;

        std::cout << "[Editor] Initializing editor...\n";

        if (P32::Engine::LoadDLL(GetDLLPath())) {
            engine_ready = true;
        }

        if (engine_ready) {
            int engineID = P32::Engine::CreateEngine(1024, 768, "Editor - Project32");

            if (engineID != -1) {
                std::cout << "Created engine with ID: " << engineID << std::endl;

                P32::Engine::ToggleEngineRenderScene(engineID);

                P32::Engine::RunEngine(engineID);

                P32::Engine::DestroyEngine(engineID);
            }
            else {
                std::cerr << "Failed to create engine!" << std::endl;
            }

            editor_initialized = true;
            std::cout << "[Editor] Editor initialized!\n";
        }
    }

    void ShutdownEditor() {
        if (!editor_initialized) return;

        std::cout << "[Editor] Shutting down...\n";
        editor_initialized = false;
        std::cout << "[Editor] Shutdown complete!\n";
    }

    void EditorTick(float deltaTime) {
        std::cout << "[Editor] Tick: " << deltaTime << "s\n";
    }

    void RenderEditor() {}
}
