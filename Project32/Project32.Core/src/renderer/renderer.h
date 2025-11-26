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
#include "lighting.h"

class Window;
class Skybox;
class Shader;

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

    void SetFOV(float fov) { m_settings.fov = glm::clamp(fov, 30.0f, 120.0f); }

    LightingSystem* GetLightingSystem() { return m_lightingSystem.get(); }

private:
    Window* m_window;
    IGraphicsBackend* m_backend;
    BackendType m_backendType;

    std::unique_ptr<TextureManager> m_textures;
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<ShadowMap> shadowMap;
    std::unique_ptr<LightingSystem> m_lightingSystem;

    CameraManager m_cameraManager;
    SceneManager& m_sceneManager;

    MeshCache m_meshCache;

    std::vector<std::unique_ptr<ModelImporter::LoadedModel>> m_loadedModels;
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<WallSystem> m_wallSystem;

    RendererData::RendererSettings m_settings;
    bool m_isReady;

    static constexpr int GRID_SIZE = 40;

    void RenderSceneObjects(Shader* shader, const glm::mat4& view,
        const glm::mat4& projection);
    void RenderDebugUI(const glm::vec3& cameraPos, const glm::vec3& cameraRot);
};

#endif // RENDERER_H