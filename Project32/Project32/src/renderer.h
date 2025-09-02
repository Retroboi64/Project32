#pragma once
#include "mesh.h"
#include "shader.h"

namespace Renderer {
    void CreateQuad();
    void CreateCube();
    void Init();
    void RenderFrame();
    void DrawHUD();
    void ToggleWireframe();
    void ToggleDebugInfo();
}
