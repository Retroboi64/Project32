#pragma once
#include "mesh.h"
#include "shader.h"

namespace Renderer {
    void Init();
    void RenderFrame();
	void LoadSkybox();
	void LoadMeshes();
	void LoadTextures();
	void LoadShaders();
	void DrawSkybox(const glm::mat4& projection, const glm::mat4& view);
    void DrawHUD();
	void Cleanup();
    void ToggleWireframe();
    void ToggleDebugInfo();
}
