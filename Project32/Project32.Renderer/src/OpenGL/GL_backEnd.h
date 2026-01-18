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

#ifndef GL_BACKEND_H
#define GL_BACKEND_H

#include "../r_common.h"
#include "../backend.h"

#include "Types/GL_shader.h"
#include "Types/GL_shadow.h"

class Mesh;
class ShadowMap;

class OpenGLBackend : public IGraphicsBackend {
public:
    OpenGLBackend() = default;
    ~OpenGLBackend() override;

    bool Init() override;
    void Shutdown() override;
    BackendType GetType() const override { return BackendType::OPENGL; }

    void BeginFrame() override;
    void EndFrame() override;

    void Clear(const glm::vec4& color) override;
    void SetViewport(int x, int y, int width, int height) override;

	ShadowMap* CreateShadowMap(unsigned int width = 2048, unsigned int height = 2048);

    void SetDepthTest(bool enabled) override;
    void SetCullFace(bool enabled) override;
    void SetWireframe(bool enabled) override;
    void SetBlending(bool enabled) override;

    void BindShader(int shaderID) override;
    void SetShaderMat4(int shaderID, const std::string& name, const glm::mat4& value) override;
    void SetShaderVec3(int shaderID, const std::string& name, const glm::vec3& value) override;
    void SetShaderFloat(int shaderID, const std::string& name, float value) override;
    void SetShaderInt(int shaderID, const std::string& name, int value) override;
    void SetShaderBool(int shaderID, const std::string& name, bool value) override;

    void BindTexture(unsigned int textureID, int slot = 0) override;
    void UnbindTexture(int slot = 0) override;

    void DrawMesh(const Mesh* mesh);
    void DrawIndexed(unsigned int vao, unsigned int indexCount) override;
    void DrawArrays(unsigned int vao, unsigned int vertexCount) override;

    unsigned int CreateBuffer() override;
    void DeleteBuffer(unsigned int bufferID) override;
    unsigned int CreateVertexArray() override;
    void DeleteVertexArray(unsigned int vaoID) override;

    std::string GetAPIVersion() const override;
    std::string GetRendererName() const override;

private:
    int m_currentShaderID = -1;
    bool m_initialized = false;

    int GetUniformLocation(int shaderID, const std::string& name);
    std::unordered_map<int, std::unordered_map<std::string, int>> m_uniformCache;
};

#endif // GL_BACKEND_H