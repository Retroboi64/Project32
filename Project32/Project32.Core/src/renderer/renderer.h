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

#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include "../common.h"
#include "../scene/wall.h"
#include "../scene/model.h"
#include "../scene/scene.h"
#include "../scene/camera.h"
#include "../core/window.h"
#include "render_data.h"

class Window;
class Skybox;

class Renderer {
public:
    explicit Renderer(Window* window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    void Init(BackendType backendType = BackendType::OPENGL);
    void RenderFrame();
    void Cleanup();

    void LoadShaders();

    bool IsReady() const { return m_isReady; }
    bool IsWireframeMode() const { return m_settings.wireframeMode; }
    bool IsDebugInfoVisible() const { return m_settings.showDebugInfo; }
    float GetFOV() const { return m_settings.fov; }
    const RendererData::RendererSettings& GetSettings() const { return m_settings; }
    BackendType GetBackendType() const { return m_backendType; }

    void SetFOV(float fov);

private:
    Window* m_window;
    IGraphicsBackend* m_backend;
    BackendType m_backendType;

    std::unique_ptr<TextureManager> m_textures;
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<ShadowMap> shadowMap;
    CameraManager m_cameraManager;
    SceneManager& m_sceneManager;

    MeshCache m_meshCache;


    std::vector<std::unique_ptr<ModelImporter::LoadedModel>> m_loadedModels;
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<WallSystem> m_wallSystem;

    RendererData::RendererSettings m_settings;
    bool m_isReady;

    static constexpr int GRID_SIZE = 40;
};

#endif // RENDERER_H