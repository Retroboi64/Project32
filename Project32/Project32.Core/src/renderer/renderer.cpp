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

#include "../common.h"
#include "renderer.h"
#include "../scene/wall.h"
#include "../scene/model.h"
#include "../scene/scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "../core/ui.h"

Renderer::Renderer(Window* window)
    : m_window(window)
    , m_backend(GraphicsBackend::Get())
    , m_backendType(BackendType::UNDEFINED)
    , m_sceneManager(SceneManager::Instance())
    , m_shaderManager(nullptr)
    , m_textures(nullptr)
    , m_isReady(false)
{
    if (!m_window) {
        throw std::runtime_error("Renderer requires a valid Window instance");
    }
}

Renderer::~Renderer() {
    Cleanup();
}

void Renderer::Init(BackendType backendType) {
    if (!m_window) {
        throw std::runtime_error("Window instance not available for Renderer initialization");
    }

    m_window->MakeContextCurrent();

    try {
        m_backend = GraphicsBackend::Get();
        m_backendType = GraphicsBackend::GetCurrentType();

        if (!m_backend) {
            throw std::runtime_error("Failed to get graphics backend instance");
        }

        m_textures = TextureFactory::CreateTextureManager();
        m_shaderManager = ShaderFactory::CreateShaderManager();

        LoadShaders();

        m_cubeMesh = MeshFactory::CreateCube();

        spdlog::info("[Renderer::Init] Renderer initialized successfully for Window {}", m_window->GetID());
        spdlog::info("[Renderer::Init] Backend: {}", m_backendType == BackendType::OPENGL ? "OpenGL" : "Unknown");
        spdlog::info("[Renderer::Init] Window size: {}x{}", m_window->GetWidth(), m_window->GetHeight());

        m_isReady = true;
    }
    catch (const std::exception& e) {
        spdlog::error("[Renderer::Init] Initialization failed: {}", e.what());
        m_isReady = false;
        throw;
    }
}

void Renderer::LoadShaders() {
    if (!m_backend) {
        spdlog::warn("[Renderer::LoadShaders] Attempted to load shaders but backend is not available");
        return;
    }

    bool success = m_shaderManager->LoadShader("solidcolor",
        "solidcolor.vert",
        "solidcolor.frag");

    if (success) {
        spdlog::info("[Renderer::LoadShaders] Successfully loaded 'solidcolor' shader");
    }
    else {
        spdlog::error("[Renderer::LoadShaders] Failed to load 'solidcolor' shader");
    }
}

void Renderer::RenderFrame() {
    if (!m_isReady || !m_backend || !m_window) {
        spdlog::warn("[Renderer] Attempted to render frame but Renderer is not ready");
        return;
    }

    m_window->MakeContextCurrent();

    int width = m_window->GetWidth();
    int height = m_window->GetHeight();

    if (width <= 0 || height <= 0) {
        spdlog::warn("[Renderer] Invalid window dimensions: {}x{}", width, height);
        return;
    }

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    m_backend->SetViewport(0, 0, width, height);
    m_backend->BeginFrame();
    m_backend->Clear(glm::vec4(m_settings.backgroundColor, 1.0f));

    if (!m_cubeMesh) {
        m_cubeMesh = MeshFactory::CreateCube();
    }

    auto shader = m_shaderManager->GetShader("solidcolor");
    if (!shader) {
        spdlog::error("[Renderer] Shader 'solidcolor' not found");
        m_backend->EndFrame();
        return;
    }

    shader->Bind();

    shader->SetVec3("color", glm::vec3(0.8f, 0.1f, 0.1f));
    shader->SetVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));
    shader->SetVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
    shader->SetBool("useTexture", false);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        aspectRatio,
        0.1f, 100.0f
    );

    static float rotation = 0.0f;
    rotation += 0.02f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, rotation, glm::vec3(0.5f, 1.0f, 0.0f));

    shader->SetMat4("model", model);
    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);

    m_cubeMesh->Draw();

    UIX* ui = m_window->GetUI();
    if (ui && ui->IsInitialized()) {
        m_window->BeginImGuiFrame();

        if (m_settings.showDebugInfo) {
            ImGui::Begin("Debug Info", &m_settings.showDebugInfo);
            ImGui::Text("Window ID: %d", m_window->GetID());
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Text("Size: %dx%d", width, height);
            ImGui::Text("Backend: %s", m_backendType == BackendType::OPENGL ? "OpenGL" : "Unknown");
            ImGui::Separator();
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", 0.0f, 0.0f, 3.0f);
            ImGui::Text("Rotation: %.2f", rotation);
            ImGui::End();
        }

        m_window->EndImGuiFrame();
    }

    m_backend->EndFrame();
}

void Renderer::Cleanup() {
    if (m_window) {
        spdlog::info("[Renderer] Cleanup called for Window {}", m_window->GetID());
        m_window->MakeContextCurrent();
    }

    m_quadMesh.reset();
    m_cubeMesh.reset();
    m_cylinderMesh.reset();
    m_sphereMesh.reset();
    m_capsuleMesh.reset();
    m_skybox.reset();
    m_wallSystem.reset();
    m_loadedModels.clear();

    m_textures.reset();
    m_shaderManager.reset();

    m_backend = nullptr;
    m_backendType = BackendType::UNDEFINED;

    m_isReady = false;

    spdlog::info("[Renderer] Cleanup completed");
}