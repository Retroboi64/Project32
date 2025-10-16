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

#include "common.h"
#include "BackEnd/backend.h"
#include "mesh.h"
#include "shader.h"
#include "textures.h"
#include "skybox.h"
#include "wall.h"
#include "model.h"
#include "scene.h"
#include "camera.h"

class WallSystem;
class Engine;
class Window;

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
    void RenderFrame();
    void Cleanup();

    void DrawSkybox(const glm::mat4& projection, const glm::mat4& view);
    void DrawWalls(Shader* shader);
    void DrawGrid(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& playerPos, Shader* shader);
    void DrawSceneObjects(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& playerPos);

    void ToggleRenderScene();

    void ToggleWireframe();
    void ToggleDebugInfo();
    void ToggleSettingsWindow();
    void ToggleImGuiDemo();

    bool IsReady() const { return m_isReady; }
    bool IsWireframeMode() const { return m_settings.wireframeMode; }
    bool IsDebugInfoVisible() const { return m_settings.showDebugInfo; }
    float GetFOV() const { return m_settings.fov; }
    const RenderSettings& GetSettings() const { return m_settings; }
    BackendType GetBackendType() const { return m_backendType; }

    void SetFOV(float fov);
    void SetBackgroundColor(const glm::vec3& color);
    void SetLightPosition(const glm::vec3& position);

private:
    void LoadShaders();
    void LoadMeshes();
    void LoadTextures();
    void LoadSkybox();
    void LoadModels();
    void LoadLevel();
    void LoadScene();

    void RenderUI(Window* window);
    void DrawImGuiHUD();
    void DrawSettingsWindow();

    void SetupRenderState();
    glm::mat4 CalculateProjectionMatrix(const glm::ivec2& windowSize) const;
    glm::mat4 CalculateViewMatrix(const glm::vec3& position) const;

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