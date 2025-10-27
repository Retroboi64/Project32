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
        // m_shaderManager = ShaderFactory::CreateShaderManager();

        m_isReady = true;
    }
    catch (const std::exception& e) {
        m_isReady = false;
        throw e;
    }
}

void Renderer::RenderFrame() {
    if (!m_isReady || !m_backend) {
        spdlog::warn("[Renderer] Attempted to render frame but Renderer is not ready");
        return;
    }
    m_backend->BeginFrame();
	m_backend->Clear(glm::vec4(m_settings.backgroundColor, 1.0f));
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