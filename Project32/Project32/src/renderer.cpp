#include "renderer.h"
#include "common.h"
#include "shader.h"
#include "skybox.h"
#include "mesh.h"
#include "game.h"
#include "GL.h"

namespace Renderer {
    std::unique_ptr<Shader> _solidColorShader;
    std::unique_ptr<Shader> _wireframeShader;
    std::unique_ptr<Shader> _skyboxShader;
    std::unique_ptr<Mesh> _quadMesh;
    std::unique_ptr<Mesh> _cubeMesh;
	std::unique_ptr<Skybox> _skybox;
    bool _wireframeMode = false;
    bool _showDebugInfo = true;

    void CreateQuad() {
        std::vector<Vertex> vertices = {
            {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
        };
        std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

        _quadMesh = std::make_unique<Mesh>();
        _quadMesh->LoadData(vertices, indices);
    }

    void CreateCube() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Define cube vertices with proper winding order (counter-clockwise when viewed from outside)
        std::vector<Vertex> cubeVertices = {
            // Back face (looking at -Z)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},

            // Front face (looking at +Z)
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},

            // Left face (looking at -X)
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},

            // Right face (looking at +X)
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            // Bottom face (looking at -Y)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},

            // Top face (looking at +Y)
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
        };

        // Indices with proper counter-clockwise winding
        std::vector<unsigned int> cubeIndices = {
            // Back face
            0, 1, 2,  2, 3, 0,
            // Front face  
            4, 5, 6,  6, 7, 4,
            // Left face
            8, 9, 10,  10, 11, 8,
            // Right face
            12, 13, 14,  14, 15, 12,
            // Bottom face
            16, 17, 18,  18, 19, 16,
            // Top face
            20, 21, 22,  22, 23, 20
        };

        _cubeMesh = std::make_unique<Mesh>();
        _cubeMesh->LoadData(cubeVertices, cubeIndices);
    }

    void Init() {
        _solidColorShader = std::make_unique<Shader>();
        _wireframeShader = std::make_unique<Shader>();
		_skyboxShader = std::make_unique<Shader>();

        _solidColorShader->Load("solidcolor.vert", "solidcolor.frag");
        _wireframeShader->Load("solidcolor.vert", "wireframe.frag");
        _skyboxShader->Load("skybox.vert", "skybox.frag");

        CreateQuad();
        CreateCube();

        _skybox = std::make_unique<Skybox>();
        std::vector<std::string> faces = {
            "res/skybox/right.jpg", "res/skybox/left.jpg", "res/skybox/top.jpg", "res/skybox/bottom.jpg", "res/skybox/front.jpg", "res/skybox/back.jpg"
        };
        _skybox->Load(faces);
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

        if (_wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

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

        std::vector<glm::vec3> cubePositions = {
            glm::vec3(0, 1, -5),    glm::vec3(3, 1, -8),    glm::vec3(-4, 1, -3),
            glm::vec3(6, 1, 2),     glm::vec3(-2, 1, 7),    glm::vec3(8, 2, -2),
            glm::vec3(-6, 3, -10),  glm::vec3(12, 1, 5),    glm::vec3(-8, 2, 12),
            glm::vec3(15, 4, -7)
        };

        for (const auto& pos : cubePositions) {
            Transform cube;
            cube.position = pos;
            cube.scale = glm::vec3(2.0f);
            _solidColorShader->SetMat4("model", cube.ToMatrix());
            _solidColorShader->SetVec3("color", glm::vec3(0.8f, 0.3f, 0.2f));
            _cubeMesh->Draw();
        }

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

    void DrawHUD() {
        glDisable(GL_DEPTH_TEST);

        _wireframeShader->Bind();
        glm::ivec2 windowSize = GL::GetWindowSize();
        glm::mat4 hudProjection = glm::ortho(0.0f, static_cast<float>(windowSize.x),
            0.0f, static_cast<float>(windowSize.y));
        _wireframeShader->SetMat4("projection", hudProjection);
        _wireframeShader->SetMat4("view", glm::mat4(1.0f));

        float speed = Game::GetSpeed();
        float speedBarLength = std::min(speed * 8.0f, 300.0f);

        Transform speedBar;
        speedBar.position = glm::vec3(50.0f, windowSize.y - 50.0f, 0.0f);
        speedBar.scale = glm::vec3(speedBarLength, 15.0f, 1.0f);
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

    void ToggleWireframe() { _wireframeMode = !_wireframeMode; }
    void ToggleDebugInfo() { _showDebugInfo = !_showDebugInfo; }
}
