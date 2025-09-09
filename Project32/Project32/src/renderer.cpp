#include "renderer.h"
#include "common.h"
#include "shader.h"
#include "skybox.h"
#include "mesh.h"
#include "game.h"
#include "GL.h"
#include "textures.h"
#include "wall.h" // Add this include

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

    void Init() {
        LoadShaders();
        LoadMeshes();
        LoadTextures();
        LoadShaders();
        LoadSkybox();

		// TODO: MOVE THIS SOMEWHERE ELSE
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
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::ivec2 windowSize = GL::GetWindowSize();
        glm::mat4 projection = glm::perspective(glm::radians(90.0f),
            static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = Game::GetViewMatrix();
        glm::vec3 playerPos = Game::GetPosition();
        glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);

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

        // Draw floor grid
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

        // Draw walls
        DrawWalls();

		// Various positions for test cubes
        std::vector<glm::vec3> cubePositions = {
            glm::vec3(0, 1, -5),    glm::vec3(3, 1, -8),    glm::vec3(-4, 1, -3),
            glm::vec3(6, 1, 2),     glm::vec3(-2, 1, 7),    glm::vec3(8, 2, -2),
            glm::vec3(-6, 3, -10),  glm::vec3(12, 1, 5),    glm::vec3(-8, 2, 12),
            glm::vec3(15, 4, -7)
        };

		// Draw test cubes
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

        if (_showDebugInfo) {
            DrawHUD();
        }
    }

    void DrawSkybox(const glm::mat4& projection, const glm::mat4& view) {
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

        _skyboxShader->Bind();
        _skyboxShader->SetMat4("view", skyboxView);
        _skyboxShader->SetMat4("projection", projection);
        _skyboxShader->SetInt("skybox", 0);

        _skybox->Draw();
    }

    void DrawHUD() { // TODO: Refactor HUD rendering to a separate class/file
        glDisable(GL_DEPTH_TEST);

        _wireframeShader->Bind();
        glm::ivec2 windowSize = GL::GetWindowSize();
        glm::mat4 hudProjection = glm::ortho(0.0f, static_cast<float>(windowSize.x),
            0.0f, static_cast<float>(windowSize.y));
        _wireframeShader->SetMat4("projection", hudProjection);
        _wireframeShader->SetMat4("view", glm::mat4(1.0f));

        float speed = Game::GetSpeed();
        float speedBarLength = std::min(speed * 8.0f, 300.0f);

        Transform speedBar{
            .position = glm::vec3(50.0f, windowSize.y - 50.0f, 0.0f),
            .scale = glm::vec3(speedBarLength, 15.0f, 1.0f),
        };
        _wireframeShader->SetMat4("model", speedBar.ToMatrix());

        glm::vec3 speedColor = speed > 15.0f ? glm::vec3(1.0f, 0.2f, 0.2f) :
            speed > 10.0f ? glm::vec3(1.0f, 1.0f, 0.2f) :
            glm::vec3(0.2f, 1.0f, 0.2f);
        _wireframeShader->SetVec3("color", speedColor);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        _quadMesh->Draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);
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

        _textures.Clear(); 
        _wallSystem = nullptr; 
    }

    void ToggleWireframe() { _wireframeMode = !_wireframeMode; }
    void ToggleDebugInfo() { _showDebugInfo = !_showDebugInfo; }
}