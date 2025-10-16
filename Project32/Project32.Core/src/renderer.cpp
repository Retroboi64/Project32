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

#include "common.h"
#include "renderer.h"
#include "BackEnd/backend.h"
#include "shader.h"
#include "skybox.h"
#include "mesh.h"
#include "textures.h"
#include "wall.h"
#include "imgui.h"
#include "model.h"
#include "scene.h"
#include "engine.h"
#include "window.h"

Renderer::Renderer(Window* window)
    : m_window(window)
    , m_backend(nullptr)
    , m_backendType(BackendType::UNDEFINED)
    , m_sceneManager(SceneManager::Instance())
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

        LoadShaders();
        LoadMeshes();
        LoadTextures();
        LoadSkybox();
        LoadLevel();
        LoadModels();
        LoadScene();

        // **MOVE THIS**
        m_cameraManager.CreateCamera("main", glm::vec3(0.0f));

        m_isReady = true;
    }
    catch (const std::exception& e) {
        m_isReady = false;
        throw e;
    }
}

void Renderer::LoadShaders() {
    m_shaderManager.LoadShader("SolidColor", "solidcolor.vert", "solidcolor.frag");
    m_shaderManager.LoadShader("Wireframe", "solidcolor.vert", "wireframe.frag");
    m_shaderManager.LoadShader("Skybox", "skybox.vert", "skybox.frag");
}

void Renderer::LoadMeshes() {
    m_quadMesh = StaticMeshes::GetQuad();
    m_cubeMesh = StaticMeshes::GetCube();
    m_cylinderMesh = StaticMeshes::GetCylinder(16, 2.0f, 0.5f);
    m_sphereMesh = StaticMeshes::GetSphere(16, 16, 0.5f);
    m_capsuleMesh = StaticMeshes::GetCapsule(16, 8, 2.0f, 0.5f);
}

void Renderer::LoadTextures() {
    m_textures.LoadTexture("Wall", "res/textures/wall.jpg", true);
}

void Renderer::LoadSkybox() {
    m_skybox = std::make_unique<Skybox>();
    std::vector<std::string> faces = {
        "res/skybox/right.jpg", "res/skybox/left.jpg", "res/skybox/top.jpg",
        "res/skybox/bottom.jpg", "res/skybox/front.jpg", "res/skybox/back.jpg"
    };
    m_skybox->Load(faces);
}

void Renderer::LoadModels() {
    m_loadedModels.emplace_back(ModelImporter::LoadFromFile("res/models/Sphere.obj"));
}

void Renderer::LoadLevel() {
    m_wallSystem = std::make_unique<WallSystem>();
    m_wallSystem->CreateMaze();
}

void Renderer::LoadScene() {
    if (m_sceneManager.LoadScene("res/scene.json")) {
        if (auto* currentScene = m_sceneManager.GetCurrentScene()) {
            currentScene->DebugPrint();
            currentScene->PrintStatistics();
        }
    }
    else {
        auto* defaultScene = m_sceneManager.CreateScene("Default Scene");
    }
}

void Renderer::SetupRenderState() {
    m_backend->SetDepthTest(true);
    m_backend->SetCullFace(true);
}

glm::mat4 Renderer::CalculateProjectionMatrix(const glm::ivec2& windowSize) const {
    float aspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    return glm::perspective(glm::radians(m_settings.fov), aspect, NEAR_PLANE, FAR_PLANE);
}

glm::mat4 Renderer::CalculateViewMatrix(const glm::vec3& position) const {
    Transform transform{ .position = position };
    return transform.ToMatrix();
}

void Renderer::RenderFrame() {
    if (!m_isReady || !m_window) {
        return;
    }

    // Safety check for backend
    if (!m_backend) {
        std::cerr << "[Renderer] Backend is null, cannot render!" << std::endl;
        return;
    }

    m_backend->BeginFrame();

    glm::vec4 bgColor(m_settings.backgroundColor, 1.0f);
    m_backend->Clear(bgColor);

    SetupRenderState();

    glm::ivec2 windowSize = m_window->GetSize();
    m_backend->SetViewport(0, 0, windowSize.x, windowSize.y);

    glm::mat4 projection = CalculateProjectionMatrix(windowSize);

    glm::vec3 mainCameraPos = m_cameraManager.GetActiveCamera()->GetTransform().position;
    glm::mat4 view = CalculateViewMatrix(mainCameraPos);

    DrawSkybox(projection, view);

    auto* shader = m_shaderManager.GetShader("SolidColor");
    if (!shader) {
        return;
    }

    shader->Bind();
    shader->SetMat4("projection", projection);
    shader->SetMat4("view", view);
    shader->SetVec3("lightPos", m_settings.lightPosition);
    shader->SetVec3("viewPos", mainCameraPos);
    shader->SetInt("uTexture", 0);

    m_backend->SetWireframe(m_settings.wireframeMode);

    if (m_settings.renderScene)
    {
        DrawGrid(projection, view, mainCameraPos, shader);
        DrawWalls(shader);
        DrawSceneObjects(projection, view, mainCameraPos);
    }

    m_backend->SetWireframe(false);

    RenderUI(m_window);

    m_backend->EndFrame();
}

void Renderer::DrawSkybox(const glm::mat4& projection, const glm::mat4& view) {
    if (!m_skybox) {
        return;
    }

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

    auto* skyboxShader = m_shaderManager.GetShader("Skybox");
    if (!skyboxShader) {
        return;
    }

    skyboxShader->Bind();
    skyboxShader->SetMat4("view", skyboxView);
    skyboxShader->SetMat4("projection", projection);
    skyboxShader->SetInt("skybox", 0);

    m_skybox->Draw();
}

void Renderer::DrawGrid(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& playerPos, Shader* shader) {
    if (!shader) {
        return;
    }

    shader->Bind();
    shader->SetBool("useTexture", false);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    constexpr int HALF_GRID = GRID_SIZE / 2;

    for (int x = -HALF_GRID; x < HALF_GRID; x++) {
        for (int z = -HALF_GRID; z < HALF_GRID; z++) {
            Transform gridSquare;
            gridSquare.position = glm::vec3(x, 0, z);
            gridSquare.scale = glm::vec3(1.0f, 0.1f, 1.0f);

            shader->SetMat4("model", gridSquare.ToMatrix());

            bool isLight = ((x + z) % 2) == 0;
            shader->SetVec3("color", isLight ? LIGHT_SQUARE : DARK_SQUARE);

            if (m_quadMesh) {
                m_backend->DrawMesh(m_quadMesh.get());
            }
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Renderer::DrawWalls(Shader* shader) {
    if (!m_wallSystem || !shader) {
        return;
    }

    shader->SetBool("useTexture", true);
    m_backend->BindTexture(m_textures.FindTextureByName("Wall"), 0);

    const auto& walls = m_wallSystem->GetWalls();
    for (const auto& wall : walls) {
        Transform wallTransform = wall.GetTransform();
        shader->SetMat4("model", wallTransform.ToMatrix());
        shader->SetVec3("color", wall.color);

        if (m_cubeMesh) {
            m_backend->DrawMesh(m_cubeMesh.get());
        }
    }
}

void Renderer::DrawSceneObjects(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& playerPos) {
    Scene* currentScene = m_sceneManager.GetCurrentScene();
    if (!currentScene) {
        return;
    }

    auto* shader = m_shaderManager.GetShader("SolidColor");
    if (!shader) {
        return;
    }

    shader->Bind();
    shader->SetBool("useTexture", true);
    shader->SetVec3("color", glm::vec3(0.2f, 0.8f, 0.3f));

    int objectCount = static_cast<int>(currentScene->GetObjectCount());
    for (int i = 0; i < objectCount; ++i) {
        auto* obj = currentScene->FindObject(i);
        if (!obj) {
            continue;
        }

        glm::mat4 modelMatrix = obj->GetTransform().ToMatrix();
        shader->SetMat4("model", modelMatrix);
        currentScene->RenderObject(i);
    }
}

void Renderer::RenderUI(Window* window) {
    if (!window) {
        return;
    }

    window->BeginImGuiFrame();

    if (m_settings.showDebugInfo) {
        DrawImGuiHUD();
    }

    if (m_settings.showSettingsWindow) {
        DrawSettingsWindow();
    }

    if (m_settings.showImGuiDemo) {
        ImGui::ShowDemoWindow(&m_settings.showImGuiDemo);
    }

    window->EndImGuiFrame();
}

void Renderer::DrawImGuiHUD() {
    std::string windowTitle = "Debug Info - Engine " + std::to_string(m_window->GetID());
    ImGui::Begin(windowTitle.c_str(), &m_settings.showDebugInfo, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate,
        1000.0f / ImGui::GetIO().Framerate);

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Render Settings")) {
        if (ImGui::Button(m_settings.wireframeMode ? "Disable Wireframe" : "Enable Wireframe")) {
            ToggleWireframe();
        }

        ImGui::SameLine();
        if (ImGui::Button("Toggle Settings")) {
            ToggleSettingsWindow();
        }

        if (ImGui::Button("Show ImGui Demo")) {
            ToggleImGuiDemo();
        }
    }

    if (ImGui::CollapsingHeader("System Info")) {
        // Display backend-agnostic info
        const char* backendName = "Unknown";
        switch (m_backendType) {
        case BackendType::OPENGL: backendName = "OpenGL"; break;
        case BackendType::VULKAN: backendName = "Vulkan"; break;
        case BackendType::DX11: backendName = "DirectX 11"; break;
        case BackendType::DX12: backendName = "DirectX 12"; break;
        default: backendName = "Undefined"; break;
        }

        ImGui::Text("Backend: %s", backendName);
        ImGui::Text("API Version: %s", m_backend->GetAPIVersion().c_str());
        ImGui::Text("Renderer: %s", m_backend->GetRendererName().c_str());

        if (m_window) {
            glm::ivec2 windowSize = m_window->GetSize();
            ImGui::Text("Window: %dx%d", windowSize.x, windowSize.y);
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
    std::string windowTitle = "Settings - Engine " + std::to_string(m_window->GetID());
    ImGui::Begin(windowTitle.c_str(), &m_settings.showSettingsWindow);

    if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 bgColor = m_settings.backgroundColor;
        if (ImGui::ColorEdit3("Background Color", &bgColor.r)) {
            SetBackgroundColor(bgColor);
        }

        float fov = m_settings.fov;
        if (ImGui::SliderFloat("FOV", &fov, 45.0f, 120.0f)) {
            SetFOV(fov);
        }

        if (m_window) {
            bool vsync = m_window->IsVSync();
            if (ImGui::Checkbox("V-Sync", &vsync)) {
                m_window->SetVSync(vsync);
            }
        }
    }

    if (ImGui::CollapsingHeader("Lighting")) {
        glm::vec3 lightPos = m_settings.lightPosition;
        if (ImGui::SliderFloat3("Light Position", &lightPos.x, -20.0f, 20.0f)) {
            SetLightPosition(lightPos);
        }
    }

    if (ImGui::CollapsingHeader("Game")) {
        if (ImGui::Button("Regenerate Maze")) {
            if (m_wallSystem) {
                m_wallSystem->CreateMaze();
            }
        }
    }

    if (ImGui::CollapsingHeader("Engine Controls")) {
        if (ImGui::Button("Shutdown This Engine")) {
            m_window->Shutdown();
        }

        ImGui::Separator();

        EngineManager* manager = EngineManager::Instance();
        if (manager) {
            if (ImGui::Button("Create New Engine")) {
                static int engineCounter = 1;
                std::string title = "New Engine " + std::to_string(engineCounter++);
                int newEngineID = manager->CreateEngine(title);
                if (newEngineID != -1) {
                    std::cout << "Created new engine with ID: " << newEngineID << std::endl;
                }
            }

            if (manager->GetEngineCount() > 1) {
                ImGui::SameLine();
                if (ImGui::Button("Destroy Other Engines")) {
                    auto engines = manager->GetAllEngines();
                    for (Engine* engine : engines) {
                        if (engine && engine->GetID() != m_window->GetID()) {
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
    std::cout << "[Renderer] Cleaning up for Engine " << m_window->GetID() << std::endl;

    m_quadMesh.reset();
    m_cubeMesh.reset();
    m_cylinderMesh.reset();
    m_sphereMesh.reset();
    m_capsuleMesh.reset();
    m_skybox.reset();
    m_wallSystem.reset();
    m_loadedModels.clear();

    m_textures.Clear();
    m_shaderManager.Clear();

    // Backend is managed globally, don't delete it here
    m_backend = nullptr;
    m_backendType = BackendType::UNDEFINED;

    m_isReady = false;
}

void Renderer::ToggleRenderScene() {
    m_settings.renderScene = !m_settings.renderScene;
}

void Renderer::ToggleWireframe() {
    m_settings.wireframeMode = !m_settings.wireframeMode;
}

void Renderer::ToggleDebugInfo() {
    m_settings.showDebugInfo = !m_settings.showDebugInfo;
}

void Renderer::ToggleSettingsWindow() {
    m_settings.showSettingsWindow = !m_settings.showSettingsWindow;
}

void Renderer::ToggleImGuiDemo() {
    m_settings.showImGuiDemo = !m_settings.showImGuiDemo;
}

void Renderer::SetFOV(float fov) {
    m_settings.fov = glm::clamp(fov, 45.0f, 120.0f);
}

void Renderer::SetBackgroundColor(const glm::vec3& color) {
    m_settings.backgroundColor = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
}

void Renderer::SetLightPosition(const glm::vec3& position) {
    m_settings.lightPosition = position;
}