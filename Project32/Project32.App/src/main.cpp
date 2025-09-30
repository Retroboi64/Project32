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

#include <windows.h>
#include <iostream>
#include <vector>

typedef void (*InitFunc)();
typedef void (*ShutdownFunc)();
typedef std::vector<uint8_t>(*LoadFunc)(const char*);
typedef void (*SetConfigFunc)(const char*, const char*);
typedef void (*TickFunc)(float);
typedef void (*RenderFunc)();

int main() {
    HMODULE dll = LoadLibraryA("Project32.Editor.dll");
    if (!dll) {
        std::cerr << "Failed to load EditorModule.dll\n";
        return -1;
    }

    InitFunc InitializeEditor = (InitFunc)GetProcAddress(dll, "InitializeEditor");
    ShutdownFunc ShutdownEditor = (ShutdownFunc)GetProcAddress(dll, "ShutdownEditor");
    LoadFunc LoadResource = (LoadFunc)GetProcAddress(dll, "LoadResource");
    SetConfigFunc SetEditorConfig = (SetConfigFunc)GetProcAddress(dll, "SetEditorConfig");
    TickFunc EditorTick = (TickFunc)GetProcAddress(dll, "EditorTick");
    RenderFunc RenderEditor = (RenderFunc)GetProcAddress(dll, "RenderEditor");

    InitializeEditor();
    ShutdownEditor();

    FreeLibrary(dll);
    return 0;
}
