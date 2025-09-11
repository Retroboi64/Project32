#include "renderer.h"
#include "common.h"
#include "shader.h"
#include "skybox.h"
#include "mesh.h"
#include "game.h"
#include "GL.h"
#include "textures.h"
#include "wall.h"
#include "imgui.h"

namespace Renderer {
    TextureManager _textures;

    std::unique_ptr<WallSystem> _wallSystem;

    std::unique_ptr<Shader> _solidColorShader;
    std::unique_ptr<Shader> _wireframeShader;
    std::unique_ptr<Shader> _skyboxShader;

    std::unique_ptr<Mesh> _quadMesh;
    std::unique_ptr<Mesh> _cubeMesh;
    std::unique_ptr<Mesh> _cylinderMesh;
    std::unique_ptr<Mesh> _sphereMesh;
    std::unique_ptr<Mesh> _capsuleMesh;

    std::unique_ptr<Skybox> _skybox;

    bool _wireframeMode = false;
    bool _showDebugInfo = true;
    bool _showImGuiDemo = false;
    bool _showSettingsWindow = true;

    float _lightPosition[3] = { 10.0f, 10.0f, 10.0f };
    float _backgroundColor[3] = { 0.05f, 0.05f, 0.1f };
    float _fov = 90.0f;

    void Init() {
        LoadShaders();
        LoadMeshes();
        LoadTextures();
        LoadShaders();
        LoadSkybox();
        LoadLevel();
    }

    // TODO: Improve this
    void LoadLevel() {
        _wallSystem = std::make_unique<WallSystem>();
        _wallSystem->CreateMaze();
    }

    void LoadShaders() {
        _solidColorShader = std::make_unique<Shader>();
        _wireframeShader = std::make_unique<Shader>();
        _skyboxShader = std::make_unique<Shader>();

        _solidColorShader->Load("solidcolor.vert", "solidcolor.frag");
        _wireframeShader->Load("solidcolor.vert", "wireframe.frag");
        _skyboxShader->Load("skybox.vert", "skybox.frag");
    }

    void LoadSkybox() {
        _skybox = std::make_unique<Skybox>();
        std::vector<std::string> faces = {
            "res/skybox/right.jpg", "res/skybox/left.jpg", "res/skybox/top.jpg", "res/skybox/bottom.jpg", "res/skybox/front.jpg", "res/skybox/back.jpg"
        };
        _skybox->Load(faces);
    }

    void LoadMeshes() {
        _quadMesh = StaticMeshes::GetQuad();
        _cubeMesh = StaticMeshes::GetCube();
        _cylinderMesh = StaticMeshes::GetCylinder(16, 2.0f, 0.5f);
        _sphereMesh = StaticMeshes::GetSphere(16, 16, 0.5f);
        _capsuleMesh = StaticMeshes::GetCapsule(16, 8, 2.0f, 0.5f);
    }

    void LoadTextures() {
        _textures.LoadTexture("Wall", "res/textures/wall.jpg", true);
    }

    void DrawWalls() {
        if (!_wallSystem) return;

        _solidColorShader->SetBool("useTexture", true);
        _textures.BindTexture(_textures.FindTextureByName("Wall"), GL_TEXTURE0);

        const auto& walls = _wallSystem->GetWalls();
        for (const auto& wall : walls) {
            Transform wallTransform = wall.GetTransform();
            _solidColorShader->SetMat4("model", wallTransform.ToMatrix());
            _solidColorShader->SetVec3("color", wall.color);
            _cubeMesh->Draw();
        }
    }

    void RenderFrame() {
        glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::ivec2 windowSize = GL::GetWindowSize();
        glm::mat4 projection = glm::perspective(glm::radians(_fov),
            static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = Game::GetViewMatrix();
        glm::vec3 playerPos = Game::GetPosition();
        glm::vec3 lightPos = glm::vec3(_lightPosition[0], _lightPosition[1], _lightPosition[2]);

        DrawSkybox(projection, view);

        _solidColorShader->Bind();
        _solidColorShader->SetMat4("projection", projection);
        _solidColorShader->SetMat4("view", view);
        _solidColorShader->SetVec3("lightPos", lightPos);
        _solidColorShader->SetVec3("viewPos", playerPos);

        _textures.BindTexture(_textures.FindTextureByName("Wall"), GL_TEXTURE0);
        _solidColorShader->SetInt("uTexture", 0);
        _solidColorShader->SetBool("useTexture", false);

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

                _solidColorShader->SetMat4("model", gridSquare.ToMatrix());

                bool lightSquare = ((x + z) % 2) == 0;
                _solidColorShader->SetVec3("color", lightSquare ? LIGHT_SQUARE : DARK_SQUARE);

                _quadMesh->Draw();
            }
        }

        glDisable(GL_POLYGON_OFFSET_FILL);

        DrawWalls();

        std::vector<glm::vec3> cubePositions = {
            glm::vec3(0, 1, -5),    glm::vec3(3, 1, -8),    glm::vec3(-4, 1, -3),
            glm::vec3(6, 1, 2),     glm::vec3(-2, 1, 7),    glm::vec3(8, 2, -2),
            glm::vec3(-6, 3, -10),  glm::vec3(12, 1, 5),    glm::vec3(-8, 2, 12),
            glm::vec3(15, 4, -7)
        };

        const glm::vec3 cubeColor(0.8f, 0.3f, 0.2f);
        for (const auto& pos : cubePositions) {
            Transform cube{
                .position = pos,
                .scale = glm::vec3(2.0f)
            };
            _solidColorShader->SetBool("useTexture", true);
            _solidColorShader->SetMat4("model", cube.ToMatrix());
            _solidColorShader->SetVec3("color", cubeColor);
            // Dont draw cubes for now
            //_cubeMesh->Draw();
        }

        // Soon To Be Playermodel For Testing
        const glm::vec3 capsuleColor(0.2f, 0.3f, 0.8f);
        Transform capsule{
            .position = glm::vec3(0.0f, 1.0f, -2.0f),
            .scale = glm::vec3(1.0f),
        };
        _solidColorShader->SetBool("useTexture", false);
        _solidColorShader->SetMat4("model", capsule.ToMatrix());
        _solidColorShader->SetVec3("color", capsuleColor);
        _capsuleMesh->Draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Render ImGui
        GL::BeginImGuiFrame();

        if (_showDebugInfo) {
            DrawImGuiHUD();
        }

        if (_showSettingsWindow) {
            DrawSettingsWindow();
        }

        if (_showImGuiDemo) {
            ImGui::ShowDemoWindow(&_showImGuiDemo);
        }

        GL::EndImGuiFrame();
    }

    void DrawSkybox(const glm::mat4& projection, const glm::mat4& view) {
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

        _skyboxShader->Bind();
        _skyboxShader->SetMat4("view", skyboxView);
        _skyboxShader->SetMat4("projection", projection);
        _skyboxShader->SetInt("skybox", 0);

        _skybox->Draw();
    }

    void DrawImGuiHUD() {
        ImGui::Begin("Debug Info", &_showDebugInfo, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // Player info
        glm::vec3 playerPos = Game::GetPosition();
        float speed = Game::GetSpeed();
        ImGui::Separator();
        ImGui::Text("Player Position: (%.2f, %.2f, %.2f)", playerPos.x, playerPos.y, playerPos.z);
        ImGui::Text("Player Speed: %.2f", speed);

        // Speed bar visualization
        float speedNormalized = std::min(speed / 20.0f, 1.0f);
        ImVec4 speedColor = speed > 15.0f ? ImVec4(1.0f, 0.2f, 0.2f, 1.0f) :
            speed > 10.0f ? ImVec4(1.0f, 1.0f, 0.2f, 1.0f) :
            ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, speedColor);
        ImGui::ProgressBar(speedNormalized, ImVec2(200, 20), "");
        ImGui::PopStyleColor();

        ImGui::Separator();

        // Render settings
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

        // System info
        if (ImGui::CollapsingHeader("System Info")) {
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
            ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
            ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

            glm::ivec2 windowSize = GL::GetWindowSize();
            ImGui::Text("Window Size: %dx%d", windowSize.x, windowSize.y);
        }

        ImGui::End();
    }

    void DrawSettingsWindow() {
        ImGui::Begin("Settings", &_showSettingsWindow);

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::ColorEdit3("Background Color", _backgroundColor);
            ImGui::SliderFloat("FOV", &_fov, 45.0f, 120.0f);

            bool vsync = GL::_vsync;
            if (ImGui::Checkbox("V-Sync", &vsync)) {
                GL::SetVSync(vsync);
            }
        }

        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::SliderFloat3("Light Position", _lightPosition, -20.0f, 20.0f);
        }

        if (ImGui::CollapsingHeader("Game")) {
            if (ImGui::Button("Regenerate Maze")) {
                ImGui::Text("Maze regeneration not implemented yet");
            }
        }

        ImGui::End();
    }

    // Sets all pointers to null and deletes any allocated resources
    // TODO: Make this more robust and handle errors
    void Cleanup() {
        _solidColorShader.reset();
        _wireframeShader.reset();
        _skyboxShader.reset();
        _quadMesh.reset();
        _cubeMesh.reset();
        _cylinderMesh.reset();
        _sphereMesh.reset();
        _capsuleMesh.reset();
        _skybox.reset();
        _wallSystem.reset();

        _textures.Clear();
    }

    void ToggleWireframe() { _wireframeMode = !_wireframeMode; }
    void ToggleDebugInfo() { _showDebugInfo = !_showDebugInfo; }
}