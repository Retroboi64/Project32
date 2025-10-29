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

        m_isReady = true;

        LoadShaders();
    }
    catch (const std::exception& e) {
        m_isReady = false;
        throw e;
    }
}

// TODO: Load shaders from a configuration file or directory
//  based on projects configuration
// 
// [WARNING] Remember res/shaders/
//  is added to the start of the path internally
void Renderer::LoadShaders() {
    if (!m_isReady || !m_backend) {
        spdlog::warn("[Renderer::LoadShaders] Attempted to load shaders but Renderer is not ready");
        return;
    }
	bool success = m_shaderManager->LoadShader("solidcolor", 
        "solidcolor.vert", 
        "solidcolor.frag");
    if (success) {
        spdlog::info("[Renderer::LoadShaders] Successfully loaded shader");
    } else {
        spdlog::error("[Renderer::LoadShaders] Failed to load shader");
    }
}

void Renderer::RenderFrame() {
    if (!m_isReady || !m_backend) {
        spdlog::warn("[Renderer] Attempted to render frame but Renderer is not ready");
        return;
    }

    m_backend->BeginFrame();

	m_backend->Clear(glm::vec4(m_settings.backgroundColor, 1.0f));

    auto shader = m_shaderManager->GetShader("solidcolor");
    shader->Bind();

    shader->SetVec3("color", glm::vec3(0.8f, 0.1f, 0.1f));
    shader->SetVec3("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));
    shader->SetVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
    shader->SetBool("useTexture", false);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f), 
        glm::vec3(0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f)  
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),          
        800.0f / 600.0f,              
        0.1f, 100.0f                  
    );

    shader->SetMat4("model", glm::mat4(1.0f));
    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);

	m_cubeMesh = MeshFactory::CreateCube();
	m_cubeMesh->Draw();

	m_backend->EndFrame();
}

void Renderer::Cleanup() {
	spdlog::info("[Renderer] Cleanup called for Engine {}", m_window->GetID());

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

    // Note: Backend is managed globally, don't delete it here
    m_backend = nullptr;
    m_backendType = BackendType::UNDEFINED;

    m_isReady = false;
}