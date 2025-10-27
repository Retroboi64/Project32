#ifndef TYPES_H
#define TYPES_H

#include "../common.h"
#include "common.h"

#include "OpenGL/GL_common.h"

struct Vertex;
class Mesh;
class Shader;
class ShaderManager;
class Texture;
class TextureManager;
class Skybox;

namespace GraphicsTypes {
    void Initialize();
    void Shutdown();
    bool IsInitialized();
}

namespace MeshFactory {
    std::unique_ptr<Mesh> CreateQuad();
    std::unique_ptr<Mesh> CreateCube();
    std::unique_ptr<Mesh> CreateCylinder(unsigned int segments = 16, float height = 1.0f, float radius = 0.5f);
    std::unique_ptr<Mesh> CreateSphere(unsigned int latitudeSegments = 16, unsigned int longitudeSegments = 16, float radius = 0.5f);
    std::unique_ptr<Mesh> CreateCapsule(unsigned int segments = 16, unsigned int rings = 8, float height = 2.0f, float radius = 0.5f);
}

namespace ShaderFactory {
    std::unique_ptr<Shader> CreateShader();
    std::unique_ptr<ShaderManager> CreateShaderManager();
}

namespace TextureFactory {
    std::unique_ptr<Texture> CreateTexture(const std::string& name = "");
    std::unique_ptr<TextureManager> CreateTextureManager();
}

namespace SkyboxFactory {
    std::unique_ptr<Skybox> CreateSkybox();
}

#endif // TYPES_H