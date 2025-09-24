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

int main() {
    std::wcout << L"Loading engine DLL..." << std::endl;

    if (P32::Engine::LoadDLL(L"Project32.Core.dll")) {
        if (P32::Engine::Init(1024, 768, "My C++ Game")) {
            std::cout << "Engine initialized with C++ wrapper!" << std::endl;

            while (P32::Engine::IsRunning()) {
                P32::Engine::Run();
            }

            P32::Engine::Shutdown();
        }

        P32::Engine::UnloadDLL();
    }

    return 0;
}