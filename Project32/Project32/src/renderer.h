#pragma once
#include "mesh.h"
#include "shader.h"

namespace Renderer {
    void Init();
    void RenderFrame();
	void DrawSkybox(const glm::mat4& projection, const glm::mat4& view);
    void DrawHUD();
    void ToggleWireframe();
    void ToggleDebugInfo();
}
