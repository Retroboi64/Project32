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

		ScriptSystem* scriptSystem = EngineManager::Instance()->GetCurrentEngine()->GetScriptSystem();
        if (scriptSystem) {
            scriptSystem;
        }

        m_meshCache.Add("cube", MeshFactory::CreateCube());

        spdlog::info("[Renderer::Init()] Pre-loaded {} meshes", m_meshCache.Size());

        size_t memUsage = m_meshCache.GetTotalMemoryUsage();
        spdlog::info("[Renderer::Init] Total mesh memory: {} KB", memUsage / 1024);

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

    auto shader = m_shaderManager->GetShader("solidcolor");
    if (!shader) {
        m_backend->EndFrame();
        return;
    }
    shader->Bind();

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.7f, 5.0f);
    glm::vec3 cameraRot = glm::vec3(0.0f, 0.0f, 0.0f);

    if (m_window) {
        Engine* engine = m_window->GetEngine();
        if (engine) {
            ScriptSystem* scripts = engine->GetScriptSystem();
            if (scripts) {
                auto* playerScript = scripts->GetScript(0);
                if (playerScript && playerScript->IsLoaded()) {
                    sol::table player = playerScript->GetScriptTable();

                    if (player["isCamera"].valid() && player["isCamera"].get<bool>()) {
                        if (player["cameraPosition"].valid()) {
                            cameraPos = player["cameraPosition"].get<glm::vec3>();
                        }
                        if (player["cameraRotation"].valid()) {
                            cameraRot = player["cameraRotation"].get<glm::vec3>();
                        }
                    }
                }
            }
        }
    }

    float pitchRad = glm::radians(cameraRot.x);
    float yawRad = glm::radians(cameraRot.y);

    glm::vec3 forward;
    forward.x = cos(pitchRad) * sin(yawRad);
    forward.y = -sin(pitchRad);
    forward.z = -cos(pitchRad) * cos(yawRad);

    glm::vec3 target = cameraPos + glm::normalize(forward);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(cameraPos, target, up);
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f), 
        aspectRatio,
        0.1f, 1000.0f
    );

    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);
    shader->SetVec3("viewPos", cameraPos);
    shader->SetVec3("lightPos", glm::vec3(5.0f, 10.0f, 5.0f));
    shader->SetBool("useTexture", false);

    if (m_window) {
        Engine* engine = m_window->GetEngine();
        if (engine) {
            ScriptSystem* scripts = engine->GetScriptSystem();

            if (scripts) {
                for (int id = 0; id < 10; id++) {
                    auto* script = scripts->GetScript(id);
                    if (!script || !script->IsLoaded()) continue;

                    sol::table obj = script->GetScriptTable();

                    if (obj["isCamera"].valid() && obj["isCamera"].get<bool>()) {
                        continue;
                    }

                    if (!obj["position"].valid()) continue;

                    glm::vec3 pos = obj["position"].get<glm::vec3>();
                    glm::vec3 color = obj["color"].valid() ?
                        obj["color"].get<glm::vec3>() : glm::vec3(1.0f);

                    shader->SetVec3("color", color);

                    glm::vec3 rotation = obj["rotation"].valid() ?
                        obj["rotation"].get<glm::vec3>() : glm::vec3(0.0f);

                    if (obj["size"].valid()) {
                        glm::vec3 size = obj["size"].get<glm::vec3>();

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, pos);

                        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
                        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
                        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

                        model = glm::scale(model, size);

                        shader->SetMat4("model", model);
                        m_meshCache.Get("cube")->Draw();
                    }
                    else if (obj["radius"].valid()) {
                        float radius = obj["radius"].get<float>();

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, pos);
                        model = glm::scale(model, glm::vec3(radius));

                        shader->SetMat4("model", model);

                        auto* sphere = m_meshCache.GetOrCreate("sphere", []() {
                            StaticMeshes::SphereParams params;
                            params.latitudeSegments = 16;
                            params.longitudeSegments = 16;
                            return StaticMeshes::GetSphere(params);
                            });
                        sphere->Draw();
                    }
                }
            }
        }
    }

    UIX* ui = m_window->GetUI();
    if (ui && ui->IsInitialized()) {
        m_window->BeginImGuiFrame();

        if (m_settings.showDebugInfo) {
            ImGui::Begin("FPS Debug", &m_settings.showDebugInfo);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)",
                cameraPos.x, cameraPos.y, cameraPos.z);
            ImGui::Text("Camera Rotation: (%.1f, %.1f, %.1f)",
                cameraRot.x, cameraRot.y, cameraRot.z);
            ImGui::Separator();
            ImGui::Text("Controls:");
            ImGui::Text("WASD - Move (auto for now)");
            ImGui::Text("Mouse - Look (auto rotate)");
            ImGui::Text("Space - Jump");
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

	m_meshCache.Clear();
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