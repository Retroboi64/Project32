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

#include "common.h"
#include "BackEnd/common.h"
#include "wall.h"
#include "model.h"
#include "scene.h"
#include "camera.h"
#include "window.h"

class Window;
class Skybox;

class Renderer {
public:
    struct RenderSettings {
        glm::vec3 backgroundColor{ 0.05f, 0.05f, 0.1f };
        glm::vec3 lightPosition{ 10.0f, 10.0f, 10.0f };

        bool renderScene{ true };

        float fov{ 90.0f };
        bool wireframeMode{ false };
        bool showDebugInfo{ true };
        bool showImGuiDemo{ false };
        bool showSettingsWindow{ true };
    };

    explicit Renderer(Window* window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    void Init(BackendType backendType = BackendType::OPENGL);
    void Cleanup();

    bool IsReady() const { return m_isReady; }
    bool IsWireframeMode() const { return m_settings.wireframeMode; }
    bool IsDebugInfoVisible() const { return m_settings.showDebugInfo; }
    float GetFOV() const { return m_settings.fov; }
    const RenderSettings& GetSettings() const { return m_settings; }
    BackendType GetBackendType() const { return m_backendType; }

    void SetFOV(float fov);

private:
    Window* m_window;
    IGraphicsBackend* m_backend;
    BackendType m_backendType;

    TextureManager m_textures;
    ShaderManager m_shaderManager;
    CameraManager m_cameraManager;
    SceneManager& m_sceneManager;

    std::unique_ptr<Mesh> m_quadMesh;
    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Mesh> m_cylinderMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_capsuleMesh;

    std::vector<std::unique_ptr<ModelImporter::LoadedModel>> m_loadedModels;
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<WallSystem> m_wallSystem;

    RenderSettings m_settings;
    bool m_isReady;

    static constexpr int GRID_SIZE = 40;
};

#endif // RENDERER_H