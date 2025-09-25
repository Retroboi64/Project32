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
#include "mesh.h"
#include "shader.h"
#include "textures.h"
#include "skybox.h"
#include "wall.h"
#include "model.h"
#include "scene.h"
#include <memory>
#include <vector>

class WallSystem;
class Engine;

class Renderer {
private:
    TextureManager _textures;
    ShaderManager _shaderManager;
    std::unique_ptr<WallSystem> _wallSystem;
    SceneManager& _sceneManager;
    Engine* _engine;

    std::unique_ptr<Mesh> _quadMesh;
    std::unique_ptr<Mesh> _cubeMesh;
    std::unique_ptr<Mesh> _cylinderMesh;
    std::unique_ptr<Mesh> _sphereMesh;
    std::unique_ptr<Mesh> _capsuleMesh;

    std::vector<std::unique_ptr<ModelImporter::LoadedModel>> _loadedModels;
    std::unique_ptr<Skybox> _skybox;

    bool _wireframeMode;
    bool _showDebugInfo;
    bool _showImGuiDemo;
    bool _showSettingsWindow;
    bool _isReady;

    float _lightPosition[3];
    float _backgroundColor[3];
    float _fov;

    void LoadModels();
    void LoadLevel();
    void LoadScene();
    void LoadShaders();
    void LoadSkybox();
    void LoadMeshes();
    void LoadTextures();
    void DrawWalls();
    void DrawImGuiHUD();
    void DrawSettingsWindow();

public:
    explicit Renderer(Engine* engine);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Init();
    void RenderFrame();
    void DrawSkybox(const glm::mat4& projection, const glm::mat4& view);
    void Cleanup();

    void ToggleWireframe();
    void ToggleDebugInfo();

    bool IsWireframeMode() const { return _wireframeMode; }
    bool IsDebugInfoVisible() const { return _showDebugInfo; }
    bool IsReady() const { return _isReady; }
    float GetFOV() const { return _fov; }

    void SetFOV(float fov) { _fov = fov; }
    void SetBackgroundColor(float r, float g, float b) {
        _backgroundColor[0] = r;
        _backgroundColor[1] = g;
        _backgroundColor[2] = b;
    }
    void SetLightPosition(float x, float y, float z) {
        _lightPosition[0] = x;
        _lightPosition[1] = y;
        _lightPosition[2] = z;
    }

    Engine* GetEngine() const { return _engine; }
};