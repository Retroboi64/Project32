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
#include "mesh.h"
#include "shader.h"

class WallSystem;

namespace Renderer {
    void Init();
    void RenderFrame();
    void LoadSkybox();
    void LoadMeshes();
    void LoadTextures();
    void LoadShaders();
    void LoadLevel();
	void LoadModels();
	void LoadScene();
    void DrawSkybox(const glm::mat4& projection, const glm::mat4& view);
    void DrawWalls();
    void Cleanup();
    void ToggleWireframe();
    void ToggleDebugInfo();
	void DrawImGuiHUD();
	void DrawSettingsWindow();
}