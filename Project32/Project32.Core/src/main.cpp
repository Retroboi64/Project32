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

#include "common.h"
#include "core/engine.h"
#include "core/window.h"

extern "C" {
    static int g_currentEngineID = -1;

    static Engine* GetCurrentEngine() {
        if (g_currentEngineID == -1) {
            return nullptr;
        }
        return EngineManager::Instance()->GetEngineByID(g_currentEngineID);
    }

    __declspec(dllexport) bool EngineInit() {
        try {
            g_currentEngineID = EngineManager::Instance()->CreateEngine("Project32");
            if (g_currentEngineID != -1) {
                EngineManager::Instance()->SetCurrentEngine(g_currentEngineID);
                return true;
            }
            return false;
        }
        catch (...) {
            g_currentEngineID = -1;
            return false;
        }
    }

    __declspec(dllexport) void EngineRun() {
        Engine* engine = GetCurrentEngine();
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
        Engine* engine = GetCurrentEngine();
        return engine && engine->IsRunning();
    }

    __declspec(dllexport) int CreateEngine(const char* title) {
        if (!title) return -1;
        int engineID = EngineManager::Instance()->CreateEngine(std::string(title));
        if (engineID != -1) {
            if (g_currentEngineID == -1) {
                g_currentEngineID = engineID;
                EngineManager::Instance()->SetCurrentEngine(engineID);
            }
        }
        return engineID;
    }

    __declspec(dllexport) bool DestroyEngine(int engineID) {
        bool result = EngineManager::Instance()->DestroyEngine(engineID);
        if (result && engineID == g_currentEngineID) {
            int managerCurrentID = EngineManager::Instance()->GetCurrentEngineID();
            g_currentEngineID = managerCurrentID;
        }
        return result;
    }

    __declspec(dllexport) void RunEngine(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (engine) {
            engine->Run();
        }
    }

    __declspec(dllexport) void RunAllEngines() {
        EngineManager* mgr = EngineManager::Instance();
        if (mgr) {
            mgr->RunAllEngines();
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
        if (EngineManager::Instance()->SetCurrentEngine(engineID)) {
            g_currentEngineID = engineID;
        }
    }

    __declspec(dllexport) int GetCurrentEngineID() {
        return g_currentEngineID;
    }

    __declspec(dllexport) void DestroyAllEngines() {
        EngineManager::Instance()->DestroyAllEngines();
        g_currentEngineID = -1;
    }

    __declspec(dllexport) void CleanupEngineManager() {
        EngineManager::DestroyInstance();
        g_currentEngineID = -1;
    }

    __declspec(dllexport) bool KeyPressed(int key) {
        Engine* engine = GetCurrentEngine();
        if (!engine) return false;
        Window* window = engine->GetMainWindow();
        return window ? window->IsKeyPressed(key) : false;
    }

    __declspec(dllexport) bool KeyDown(int key) {
        Engine* engine = GetCurrentEngine();
        if (!engine) return false;
        Window* window = engine->GetMainWindow();
        window->GetInput()->KeyDown(key);
        return false;
    }

    __declspec(dllexport) void GetMousePos(float* x, float* y) {
        if (!x || !y) return;

        *x = 0.0f;
        *y = 0.0f;

        Engine* engine = GetCurrentEngine();
        if (!engine) return;

        Window* window = engine->GetMainWindow();
        if (window) {
            glm::ivec2 pos = window->GetInput()->GetMousePosition();

            float px = static_cast<float>(pos.x);
            float py = static_cast<float>(pos.y);

            *x = px;
            *y = py;
        }
    }

    __declspec(dllexport) void GetMouseDelta(float* dx, float* dy) {
        if (!dx || !dy) return;

        *dx = 0.0f;
        *dy = 0.0f;

        Engine* engine = GetCurrentEngine();
        if (!engine) return;

        Window* window = engine->GetMainWindow();
        if (window) {
            glm::ivec2 md = window->GetInput()->GetMouseDelta();

            float mdx = static_cast<float>(md.x);
            float mdy = static_cast<float>(md.y);

            *dx = mdx;
            *dy = mdy;
        }
    }

    __declspec(dllexport) bool EngineKeyPressed(int engineID, int key) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return false;
        Window* window = engine->GetMainWindow();
        return window ? window->IsKeyPressed(key) : false;
    }

    __declspec(dllexport) bool EngineKeyDown(int engineID, int key) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return false;
        Window* window = engine->GetMainWindow();
        window->GetInput()->KeyDown(key);
        return false;
    }

    __declspec(dllexport) void EngineGetMousePos(int engineID, float* x, float* y) {
        if (!x || !y) return;

        *x = 0.0f;
        *y = 0.0f;

        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;

        Window* window = engine->GetMainWindow();
        if (window) {
            glm::ivec2 pos = window->GetInput()->GetMousePosition();

			float px = static_cast<float>(pos.x);
            float py = static_cast<float>(pos.y);

            *x = px;
            *y = py;
        }
    }

    __declspec(dllexport) void EngineGetMouseDelta(int engineID, float* dx, float* dy) {
        if (!dx || !dy) return;

        *dx = 0.0f;
        *dy = 0.0f;

        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;

        Window* window = engine->GetMainWindow();
        if (window && window->GetInput()) {
            glm::ivec2 md = window->GetInput()->GetMouseDelta();

			float mdx = static_cast<float>(md.x);
			float mdy = static_cast<float>(md.y);
            
            *dx = mdx;
            *dy = mdy;
        }
    }

    // Engine configuration functions
    __declspec(dllexport) void SetEngineBackgroundColor(int engineID, float r, float g, float b) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window && window->GetRenderer()) {}
    }

    __declspec(dllexport) void SetEngineFOV(int engineID, float fov) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window && window->GetRenderer()) {
            //window->GetRenderer()->SetFOV(fov);
        }
    }

    __declspec(dllexport) void ToggleEngineWireframe(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window && window->GetRenderer()) {
            // window->GetRenderer()->ToggleWireframe();
        }
    }

    __declspec(dllexport) void ToggleEngineDebugInfo(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window) {}
    }

    __declspec(dllexport) void ToggleEngineRenderScene(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window && window->GetRenderer()) {
             // window->GetRenderer()->ToggleRenderScene();
        }
    }

    __declspec(dllexport) bool SetEngineWindowSize(int engineID, int width, int height) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return false;
        Window* window = engine->GetMainWindow();
        if (window) {
            return window->SetSize(width, height);
        }
        return false;
    }

    __declspec(dllexport) void GetEngineWindowSize(int engineID, int* width, int* height) {
        if (!width || !height) return;

        *width = 0;
        *height = 0;

        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;

        Window* window = engine->GetMainWindow();
        if (window) {
            glm::ivec2 size = window->GetSize();

			float sx = static_cast<float>(size.x);
			float sy = static_cast<float>(size.y);

            *width = sx;
            *height = sy;
        }
    }

    __declspec(dllexport) void SetEngineWindowTitle(int engineID, const char* title) {
        if (!title) return;
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window) {
            window->SetTitle(std::string(title));
        }
    }

    __declspec(dllexport) void SetEngineVSync(int engineID, bool enabled) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        Window* window = engine->GetMainWindow();
        if (window) {
            window->SetVSync(enabled);
        }
    }

    __declspec(dllexport) bool GetEngineVSync(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return false;
        Window* window = engine->GetMainWindow();
        return window ? window->IsVSync() : false;
    }

    __declspec(dllexport) int CreateEngineWindow(int engineID, int width, int height, const char* title) {
        if (!title) return -1;
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return -1;
        WindowManager* wm = engine->GetWindowManager();
        if (!wm) return -1;
        return wm->AddWindow(width, height, std::string(title));
    }

    __declspec(dllexport) bool DestroyEngineWindow(int engineID, int windowID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return false;
        WindowManager* wm = engine->GetWindowManager();
        if (!wm) return false;
        return wm->RemoveWindow(windowID);
    }

    __declspec(dllexport) int GetEngineWindowCount(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return 0;
        WindowManager* wm = engine->GetWindowManager();
        return wm ? wm->Count() : 0;
    }

    __declspec(dllexport) void SetEngineMainWindow(int engineID, int windowID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        engine->SetMainWindow(windowID);
    }

    __declspec(dllexport) int GetEngineMainWindowID(int engineID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return -1;
        return engine->GetMainWindowID();
    }

    __declspec(dllexport) void GetWindowPosition(int engineID, int windowID, int* x, int* y) {
        if (!x || !y) return;

        *x = 0;
        *y = 0;

        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;

        WindowManager* wm = engine->GetWindowManager();
        if (!wm) return;

        Window* window = wm->GetWindowByID(windowID);
        if (window) {
            glm::ivec2 pos = window->GetPosition();

			float px = static_cast<float>(pos.x);
			float py = static_cast<float>(pos.y);

            *x = px;
            *y = py;
        }
    }

    __declspec(dllexport) void SetWindowPosition(int engineID, int windowID, int x, int y) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return;
        WindowManager* wm = engine->GetWindowManager();
        if (!wm) return;
        Window* window = wm->GetWindowByID(windowID);
        if (window) {
            window->SetPosition(x, y);
        }
    }

    __declspec(dllexport) bool IsWindowOpen(int engineID, int windowID) {
        Engine* engine = EngineManager::Instance()->GetEngineByID(engineID);
        if (!engine) return false;
        WindowManager* wm = engine->GetWindowManager();
        if (!wm) return false;
        Window* window = wm->GetWindowByID(windowID);
        return window ? window->IsOpen() : false;
    }
}