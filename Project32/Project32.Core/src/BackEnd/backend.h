#ifndef BACKEND_H
#define BACKEND_H

#include "../common.h"

class IGraphicsBackend;
class ShadowMap;

enum class BackendType {
    UNDEFINED,
    OPENGL,
    VULKAN,
    DX11,
    DX12
};

class IGraphicsBackend {
public:
    virtual ~IGraphicsBackend() = default;

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual BackendType GetType() const = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void Clear(const glm::vec4& color) = 0;
    virtual void SetViewport(int x, int y, int width, int height) = 0;

    virtual void SetDepthTest(bool enabled) = 0;
    virtual void SetCullFace(bool enabled) = 0;
    virtual void SetWireframe(bool enabled) = 0;
    virtual void SetBlending(bool enabled) = 0;

	virtual ShadowMap* CreateShadowMap(unsigned int width = 2048, unsigned int height = 2048) = 0;

    virtual void BindShader(int shaderID) = 0;
    virtual void SetShaderMat4(int shaderID, const std::string& name, const glm::mat4& value) = 0;
    virtual void SetShaderVec3(int shaderID, const std::string& name, const glm::vec3& value) = 0;
    virtual void SetShaderFloat(int shaderID, const std::string& name, float value) = 0;
    virtual void SetShaderInt(int shaderID, const std::string& name, int value) = 0;
    virtual void SetShaderBool(int shaderID, const std::string& name, bool value) = 0;

    virtual void BindTexture(unsigned int textureID, int slot = 0) = 0;
    virtual void UnbindTexture(int slot = 0) = 0;

    virtual void DrawIndexed(unsigned int vao, unsigned int indexCount) = 0;
    virtual void DrawArrays(unsigned int vao, unsigned int vertexCount) = 0;

    virtual unsigned int CreateBuffer() = 0;
    virtual void DeleteBuffer(unsigned int bufferID) = 0;
    virtual unsigned int CreateVertexArray() = 0;
    virtual void DeleteVertexArray(unsigned int vaoID) = 0;

    virtual std::string GetAPIVersion() const = 0;
    virtual std::string GetRendererName() const = 0;
};

class GraphicsBackend {
public:
    static IGraphicsBackend* Get();
    static BackendType GetCurrentType();
    static bool Initialize(BackendType type);
    static void Destroy();

private:
    GraphicsBackend() = default;
    ~GraphicsBackend() = default;

    GraphicsBackend(const GraphicsBackend&) = delete;
    GraphicsBackend& operator=(const GraphicsBackend&) = delete;

    static std::unique_ptr<IGraphicsBackend> s_instance;
    static BackendType s_currentType;
};

#endif 