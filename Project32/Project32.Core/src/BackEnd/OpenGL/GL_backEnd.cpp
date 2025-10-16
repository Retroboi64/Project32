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

#include "../../common.h"
#include "GL_backEnd.h"
#include "../../window.h"

OpenGLBackend::~OpenGLBackend() {
    Shutdown();
}

bool OpenGLBackend::Init() {
    if (m_initialized) {
        return true;
    }

    const GLubyte* version = glGetString(GL_VERSION);
    if (!version) {
        std::cerr << "Failed to get OpenGL version" << std::endl;
        return false;
    }

    std::cout << "OpenGL Backend initialized" << std::endl;
    std::cout << "  Version: " << GetAPIVersion() << std::endl;
    std::cout << "  Renderer: " << GetRendererName() << std::endl;

    SetDepthTest(true);
    SetCullFace(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    m_initialized = true;
    return true;
}

void OpenGLBackend::Shutdown() {
    if (!m_initialized) {
        return;
    }

    m_uniformCache.clear();
    m_currentShaderID = -1;
    m_initialized = false;

    std::cout << "OpenGL Backend shut down" << std::endl;
}

void OpenGLBackend::BeginFrame() {
    // Nothing specific needed for OpenGL
}

void OpenGLBackend::EndFrame() {
    // Nothing specific needed for OpenGL
}

void OpenGLBackend::Clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLBackend::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLBackend::SetDepthTest(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}

void OpenGLBackend::SetCullFace(bool enabled) {
    if (enabled) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
}

void OpenGLBackend::SetWireframe(bool enabled) {
    if (enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void OpenGLBackend::SetBlending(bool enabled) {
    if (enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        glDisable(GL_BLEND);
    }
}

void OpenGLBackend::BindShader(int shaderID) {
    if (shaderID != m_currentShaderID) {
        glUseProgram(static_cast<GLuint>(shaderID));
        m_currentShaderID = shaderID;
    }
}

int OpenGLBackend::GetUniformLocation(int shaderID, const std::string& name) {
    auto& shaderCache = m_uniformCache[shaderID];
    auto it = shaderCache.find(name);

    if (it != shaderCache.end()) {
        return it->second;
    }

    int location = glGetUniformLocation(static_cast<GLuint>(shaderID), name.c_str());
    shaderCache[name] = location;

    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader " << shaderID << std::endl;
    }

    return location;
}

void OpenGLBackend::SetShaderMat4(int shaderID, const std::string& name, const glm::mat4& value) {
    BindShader(shaderID);
    int location = GetUniformLocation(shaderID, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void OpenGLBackend::SetShaderVec3(int shaderID, const std::string& name, const glm::vec3& value) {
    BindShader(shaderID);
    int location = GetUniformLocation(shaderID, name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void OpenGLBackend::SetShaderFloat(int shaderID, const std::string& name, float value) {
    BindShader(shaderID);
    int location = GetUniformLocation(shaderID, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void OpenGLBackend::SetShaderInt(int shaderID, const std::string& name, int value) {
    BindShader(shaderID);
    int location = GetUniformLocation(shaderID, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void OpenGLBackend::SetShaderBool(int shaderID, const std::string& name, bool value) {
    BindShader(shaderID);
    int location = GetUniformLocation(shaderID, name);
    if (location != -1) {
        glUniform1i(location, static_cast<int>(value));
    }
}

void OpenGLBackend::BindTexture(unsigned int textureID, int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void OpenGLBackend::UnbindTexture(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLBackend::DrawMesh(const Mesh* mesh) {
    if (!mesh) {
        return;
    }
    mesh->Draw();
}

void OpenGLBackend::DrawIndexed(unsigned int vao, unsigned int indexCount) {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void OpenGLBackend::DrawArrays(unsigned int vao, unsigned int vertexCount) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

unsigned int OpenGLBackend::CreateBuffer() {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    return static_cast<unsigned int>(buffer);
}

void OpenGLBackend::DeleteBuffer(unsigned int bufferID) {
    GLuint buffer = static_cast<GLuint>(bufferID);
    glDeleteBuffers(1, &buffer);
}

unsigned int OpenGLBackend::CreateVertexArray() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return static_cast<unsigned int>(vao);
}

void OpenGLBackend::DeleteVertexArray(unsigned int vaoID) {
    GLuint vao = static_cast<GLuint>(vaoID);
    glDeleteVertexArrays(1, &vao);
}

std::string OpenGLBackend::GetAPIVersion() const {
    const GLubyte* version = glGetString(GL_VERSION);
    return version ? reinterpret_cast<const char*>(version) : "Unknown";
}

std::string OpenGLBackend::GetRendererName() const {
    const GLubyte* renderer = glGetString(GL_RENDERER);
    return renderer ? reinterpret_cast<const char*>(renderer) : "Unknown";
}