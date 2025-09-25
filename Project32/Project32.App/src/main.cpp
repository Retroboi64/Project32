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

#include "../../Project32.API/src/Project32.API.h"

#include <iostream>
#include <vector>
#include <thread>

void SingleEngineExample() {
    std::wcout << L"=== Single Engine Example ===" << std::endl;

    int engineID = P32::Engine::CreateEngine(1024, 768, "Project32 - Single Engine");

    if (engineID != -1) {
        std::cout << "Created engine with ID: " << engineID << std::endl;

        P32::Engine::SetEngineBackgroundColor(engineID, 0.1f, 0.2f, 0.3f);
        P32::Engine::SetEngineFOV(engineID, 75.0f);

        P32::Engine::RunEngine(engineID);

        P32::Engine::DestroyEngine(engineID);
    }
    else {
        std::cerr << "Failed to create engine!" << std::endl;
    }
}

void MultiEngineExample() {
    std::wcout << L"=== Multi-Engine Example ===" << std::endl;

    std::vector<int> engineIDs;

    engineIDs.push_back(P32::Engine::CreateEngine(800, 600, "Engine 1 - Main"));
    engineIDs.push_back(P32::Engine::CreateEngine(640, 480, "Engine 2 - Debug"));
    engineIDs.push_back(P32::Engine::CreateEngine(1024, 768, "Engine 3 - Tools"));

    std::cout << "Created " << P32::Engine::GetEngineCount() << " engines" << std::endl;

    if (engineIDs.size() >= 3) {
        // TODO: Add functions to add more control

        //P32::Engine::SetEngineBackgroundColor(engineIDs[0], 0.2f, 0.3f, 0.4f);  // Blue-ish
        //P32::Engine::SetEngineBackgroundColor(engineIDs[1], 0.4f, 0.2f, 0.3f);  // Red-ish
        //P32::Engine::SetEngineBackgroundColor(engineIDs[2], 0.3f, 0.4f, 0.2f);  // Green-ish

        //P32::Engine::SetEngineFOV(engineIDs[0], 90.0f);
        //P32::Engine::SetEngineFOV(engineIDs[1], 60.0f);
        //P32::Engine::SetEngineFOV(engineIDs[2], 75.0f);
    }

    std::vector<std::thread> threads;

    for (size_t i = 0; i < engineIDs.size() - 1 && i < 2; ++i) {
        threads.emplace_back([engineID = engineIDs[i]]() {
            std::cout << "Starting engine " << engineID << " in thread" << std::endl;
            P32::Engine::RunEngine(engineID);
            std::cout << "Engine " << engineID << " finished" << std::endl;
            });
    }

    if (!engineIDs.empty()) {
        int mainEngineID = engineIDs.back();
        std::cout << "Running engine " << mainEngineID << " on main thread" << std::endl;
        P32::Engine::RunEngine(mainEngineID);
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    for (int engineID : engineIDs) {
        P32::Engine::DestroyEngine(engineID);
    }

    std::cout << "All engines destroyed. Remaining count: " << P32::Engine::GetEngineCount() << std::endl;
}

void EngineInstanceExample() {
    std::wcout << L"=== Engine Instance Wrapper Example ===" << std::endl;

    P32::EngineInstance engine1(800, 600, "Instance 1");
    P32::EngineInstance engine2(640, 480, "Instance 2");

    if (engine1.IsValid() && engine2.IsValid()) {
        std::cout << "Created engine instances with IDs: "
            << engine1.GetID() << " and " << engine2.GetID() << std::endl;

        engine1.SetBackgroundColor(0.1f, 0.3f, 0.5f);
        engine1.SetFOV(90.0f);

        engine2.SetBackgroundColor(0.5f, 0.3f, 0.1f);
        engine2.SetFOV(60.0f);

        engine1.Run();
    }
    else {
        std::cerr << "Failed to create engine instances!" << std::endl;
    }

}

void LegacyCompatibilityExample() {
    std::wcout << L"=== Legacy Compatibility Example ===" << std::endl;

    if (P32::Engine::Init()) {
        std::cout << "Legacy engine initialized!" << std::endl;

        if (P32::Engine::IsRunning()) {
            P32::Engine::Run();
        }

        P32::Engine::Shutdown();
    }
    else {
        std::cerr << "Failed to initialize legacy engine!" << std::endl;
    }
}

int main() {
    std::wcout << L"Loading engine DLL..." << std::endl;

    if (P32::Engine::LoadDLL(L"Project32.Core.dll")) {
        SingleEngineExample();
    }

    return 0;
}