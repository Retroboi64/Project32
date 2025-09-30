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

#pragma once
#include <cstdint>
#include <vector>
#include <string>

#ifdef EDITORMODULE_EXPORTS
#define EDITORMODULE_API __declspec(dllexport)
#else
#define EDITORMODULE_API __declspec(dllimport)
#endif

extern "C" {
	EDITORMODULE_API void InitializeEditor();
	EDITORMODULE_API void ShutdownEditor();
	EDITORMODULE_API void EditorTick(float deltaTime);
	EDITORMODULE_API void RenderEditor();
}
