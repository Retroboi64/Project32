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