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

#include "renderer.h"
#include "common.h"
#include "shader.h"
#include "skybox.h"
#include "mesh.h"
#include "textures.h"
#include "wall.h"
#include "imgui.h"
#include "model.h"
#include "scene.h"
#include "engine.h"

Renderer::Renderer(Engine* engine)
    : _sceneManager(SceneManager::Instance())
    , _engine(engine)
    , _wireframeMode(false)
    , _showDebugInfo(true)
    , _showImGuiDemo(false)
    , _showSettingsWindow(true)
    , _lightPosition{ 10.0f, 10.0f, 10.0f }
    , _backgroundColor{ 0.05f, 0.05f, 0.1f }
    , _fov(90.0f)
    , _isReady(false)
{
    if (!_engine) {
        throw std::runtime_error("Renderer requires a valid Engine instance");
    }
}

Renderer::~Renderer() {
    Cleanup();
}

void Renderer::Init() {
    if (!_engine) {
        throw std::runtime_error("Engine instance not available for Renderer initialization");
    }

    // Make sure this engine's window context is current
    Window* window = _engine->GetWindow();
    if (!window) {
        throw std::runtime_error("Window not available for Renderer initialization");
    }

    window->MakeContextCurrent();

    try {
        LoadShaders();
        LoadMeshes();
        LoadTextures();
        LoadSkybox();
        LoadLevel();
        LoadModels();
        LoadScene(); // Testing with a scene system

        _isReady = true;
        std::cout << "[Renderer::Init] Renderer initialized for engine " << _engine->GetID() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[Renderer::Init] Failed to initialize renderer for engine "
            << _engine->GetID() << ": " << e.what() << std::endl;
        _isReady = false;
        throw;
    }
}

void Renderer::LoadModels() {
    _loadedModels.emplace_back(ModelImporter::LoadFromFile("res/models/Sphere.obj"));
}

// TODO: Improve this
void Renderer::LoadLevel() {
    _wallSystem = std::make_unique<WallSystem>();
    _wallSystem->CreateMaze();
}

void Renderer::LoadScene() {
    if (_sceneManager.LoadScene("res/scene.json")) {
        std::cout << "[Engine " << _engine->GetID() << "] Loaded scene from JSON file successfully" << std::endl;
        if (auto* currentScene = _sceneManager.GetCurrentScene()) {
            currentScene->DebugPrint();
            currentScene->PrintStatistics();
        }
    }
    else {
        std::cout << "[Engine " << _engine->GetID() << "] Failed to load scene from res/scene.json" << std::endl;

        auto* defaultScene = _sceneManager.CreateScene("Default Scene");
        std::cout << "[Engine " << _engine->GetID() << "] Created default scene: " << defaultScene->GetMetadata().name << std::endl;
    }
}

void Renderer::LoadShaders() {
    _shaderManager.LoadShader("SolidColor", "solidcolor.vert", "solidcolor.frag");
    _shaderManager.LoadShader("Wireframe", "solidcolor.vert", "wireframe.frag");
    _shaderManager.LoadShader("Skybox", "skybox.vert", "skybox.frag");
}

void Renderer::LoadSkybox() {
    _skybox = std::make_unique<Skybox>();
    std::vector<std::string> faces = {
        "res/skybox/right.jpg", "res/skybox/left.jpg", "res/skybox/top.jpg",
        "res/skybox/bottom.jpg", "res/skybox/front.jpg", "res/skybox/back.jpg"
    };
    _skybox->Load(faces);
}

void Renderer::LoadMeshes() {
    _quadMesh = StaticMeshes::GetQuad();
    _cubeMesh = StaticMeshes::GetCube();
    _cylinderMesh = StaticMeshes::GetCylinder(16, 2.0f, 0.5f);
    _sphereMesh = StaticMeshes::GetSphere(16, 16, 0.5f);
    _capsuleMesh = StaticMeshes::GetCapsule(16, 8, 2.0f, 0.5f);
}

void Renderer::LoadTextures() {
    _textures.LoadTexture("Wall", "res/textures/wall.jpg", true);
}

void Renderer::DrawWalls() {
    if (!_wallSystem) return;

    _shaderManager.GetShader("SolidColor")->SetBool("useTexture", true);
    _textures.BindTexture(_textures.FindTextureByName("Wall"), GL_TEXTURE0);

    const auto& walls = _wallSystem->GetWalls();
    for (const auto& wall : walls) {
        Transform wallTransform = wall.GetTransform();
        _shaderManager.GetShader("SolidColor")->SetMat4("model", wallTransform.ToMatrix());
        _shaderManager.GetShader("SolidColor")->SetVec3("color", wall.color);
        _cubeMesh->Draw();
    }
}

void Renderer::RenderFrame() {
    if (!_isReady || !_engine) return;

    Window* window = _engine->GetWindow();
    if (!window) return;

    window->MakeContextCurrent();

    glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Ensure depth testing is enabled
    glEnable(GL_DEPTH_TEST);

    Transform playerTransform{
        .position = glm::vec3(0.0f, 1.0f, 0.0f)
    };

    glm::ivec2 windowSize = window->GetSize();
    glm::mat4 projection = glm::perspective(glm::radians(_fov),
        static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), NEAR_PLANE, FAR_PLANE);
    glm::mat4 view = playerTransform.ToMatrix();
    glm::vec3 playerPos = playerTransform.position;
    glm::vec3 lightPos = glm::vec3(_lightPosition[0], _lightPosition[1], _lightPosition[2]);

    DrawSkybox(projection, view);

    auto* shader = _shaderManager.GetShader("SolidColor");
    if (!shader) return;

    shader->Bind();
    shader->SetMat4("projection", projection);
    shader->SetMat4("view", view);
    shader->SetVec3("lightPos", lightPos);
    shader->SetVec3("viewPos", playerPos);

    _textures.BindTexture(_textures.FindTextureByName("Wall"), GL_TEXTURE0);
    shader->SetInt("uTexture", 0);
    shader->SetBool("useTexture", false);

    if (_wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    constexpr int GRID_SIZE = 40;
    constexpr int HALF_GRID = GRID_SIZE / 2;

    for (int x = -HALF_GRID; x < HALF_GRID; x++) {
        for (int z = -HALF_GRID; z < HALF_GRID; z++) {
            Transform gridSquare;
            gridSquare.position = glm::vec3(x, 0, z);
            gridSquare.scale = glm::vec3(1.0f, 0.1f, 1.0f);

            shader->SetMat4("model", gridSquare.ToMatrix());

            bool lightSquare = ((x + z) % 2) == 0;
            shader->SetVec3("color", lightSquare ? LIGHT_SQUARE : DARK_SQUARE);

            if (_quadMesh) {
                _quadMesh->Draw();
            }
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);

    DrawWalls();

    Scene* currentScene = _sceneManager.GetCurrentScene();
    if (currentScene) {
        int objectCount = static_cast<int>(currentScene->GetObjectCount());

        shader->Bind();
        shader->SetBool("useTexture", true);
        shader->SetVec3("color", glm::vec3(0.2f, 0.8f, 0.3f));

        for (int i = 0; i < objectCount; ++i) {
            auto* obj = currentScene->FindObject(i);
            if (!obj) continue;

            glm::mat4 modelMatrix = obj->GetTransform().ToMatrix();
            shader->SetMat4("model", modelMatrix);

            currentScene->RenderObject(i);
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (window) {
        window->BeginImGuiFrame();

        if (_showDebugInfo) {
            DrawImGuiHUD();
        }

        if (_showSettingsWindow) {
            DrawSettingsWindow();
        }

        if (_showImGuiDemo) {
            ImGui::ShowDemoWindow(&_showImGuiDemo);
        }

        window->EndImGuiFrame();
    }
}

void Renderer::DrawSkybox(const glm::mat4& projection, const glm::mat4& view) {
    if (!_skybox) return;

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

    auto* skyboxShader = _shaderManager.GetShader("Skybox");
    if (skyboxShader) {
        skyboxShader->Bind();
        skyboxShader->SetMat4("view", skyboxView);
        skyboxShader->SetMat4("projection", projection);
        skyboxShader->SetInt("skybox", 0);

        _skybox->Draw();
    }
}

void Renderer::DrawImGuiHUD() {
    std::string windowTitle = "Debug Info - Engine " + std::to_string(_engine->GetID());
    ImGui::Begin(windowTitle.c_str(), &_showDebugInfo, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Engine ID: %d", _engine->GetID());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Render Settings")) {
        if (ImGui::Button(_wireframeMode ? "Disable Wireframe" : "Enable Wireframe")) {
            ToggleWireframe();
        }

        ImGui::SameLine();
        if (ImGui::Button("Toggle Settings")) {
            _showSettingsWindow = !_showSettingsWindow;
        }

        if (ImGui::Button("Show ImGui Demo")) {
            _showImGuiDemo = true;
        }
    }

    if (ImGui::CollapsingHeader("System Info")) {
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
        ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

        Window* window = _engine->GetWindow();
        if (window) {
            glm::ivec2 windowSize = window->GetSize();
            ImGui::Text("Window Size: %dx%d", windowSize.x, windowSize.y);
        }
    }

    if (ImGui::CollapsingHeader("Engine Manager")) {
        EngineManager* manager = EngineManager::Instance();
        if (manager) {
            ImGui::Text("Total Engines: %zu", manager->GetEngineCount());
            ImGui::Text("Current Engine: %d", manager->GetCurrentEngineID());

            if (ImGui::Button("Print Engine Info")) {
                manager->PrintEngineInfo();
            }
        }
    }

    ImGui::End();
}

void Renderer::DrawSettingsWindow() {
    std::string windowTitle = "Settings - Engine " + std::to_string(_engine->GetID());
    ImGui::Begin(windowTitle.c_str(), &_showSettingsWindow);

    if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Background Color", _backgroundColor);
        ImGui::SliderFloat("FOV", &_fov, 45.0f, 120.0f);

        Window* window = _engine->GetWindow();
        if (window) {
            bool vsync = window->IsVSync();
            if (ImGui::Checkbox("V-Sync", &vsync)) {
                window->SetVSync(vsync);
            }
        }
    }

    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::SliderFloat3("Light Position", _lightPosition, -20.0f, 20.0f);
    }

    if (ImGui::CollapsingHeader("Game")) {
        if (ImGui::Button("Regenerate Maze")) {
            if (_wallSystem) {
                _wallSystem->CreateMaze();
            }
        }
    }

    if (ImGui::CollapsingHeader("Engine Controls")) {
        if (ImGui::Button("Shutdown This Engine")) {
            _engine->Shutdown();
        }

        ImGui::Separator();

        EngineManager* manager = EngineManager::Instance();
        if (manager) {
            if (ImGui::Button("Create New Engine")) {
                static int engineCounter = 1;
                std::string title = "New Engine " + std::to_string(engineCounter++);
                int newEngineID = manager->CreateEngine(800, 600, title);
                if (newEngineID != -1) {
                    std::cout << "Created new engine with ID: " << newEngineID << std::endl;
                }
            }

            if (manager->GetEngineCount() > 1) {
                ImGui::SameLine();
                if (ImGui::Button("Destroy Other Engines")) {
                    auto engines = manager->GetAllEngines();
                    for (Engine* engine : engines) {
                        if (engine && engine->GetID() != _engine->GetID()) {
                            manager->DestroyEngine(engine->GetID());
                        }
                    }
                }
            }
        }
    }

    ImGui::End();
}

void Renderer::Cleanup() {
    std::cout << "[Renderer::Cleanup] Cleaning up renderer for engine " << _engine->GetID() << std::endl;

    _quadMesh.reset();
    _cubeMesh.reset();
    _cylinderMesh.reset();
    _sphereMesh.reset();
    _capsuleMesh.reset();
    _skybox.reset();
    _wallSystem.reset();

    _textures.Clear();
    _shaderManager.Clear();

    _isReady = false;
}

void Renderer::ToggleWireframe() {
    _wireframeMode = !_wireframeMode;
    std::cout << "[Engine " << _engine->GetID() << "] Wireframe mode: "
        << (_wireframeMode ? "ON" : "OFF") << std::endl;
}

void Renderer::ToggleDebugInfo() {
    _showDebugInfo = !_showDebugInfo;
    std::cout << "[Engine " << _engine->GetID() << "] Debug info: "
        << (_showDebugInfo ? "ON" : "OFF") << std::endl;
}