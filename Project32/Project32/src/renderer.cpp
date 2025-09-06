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
	std::unique_ptr<Mesh> _cylinderMesh;
	std::unique_ptr<Mesh> _sphereMesh;
	std::unique_ptr<Mesh> _capsuleMesh;
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

    void CreateCylinder(unsigned int segments = 32, float height = 2.0f, float radius = 0.5f) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i <= segments; ++i) {
            float theta = (float)i / segments * glm::two_pi<float>();
            float x = cos(theta);
            float z = sin(theta);
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

            glm::vec3 posTop = glm::vec3(x * radius, height * 0.5f, z * radius);
            glm::vec3 posBottom = glm::vec3(x * radius, -height * 0.5f, z * radius);

            glm::vec2 uvTop = glm::vec2((float)i / segments, 0.0f);
            glm::vec2 uvBottom = glm::vec2((float)i / segments, 1.0f);

            vertices.push_back({ posTop, normal, uvTop });
            vertices.push_back({ posBottom, normal, uvBottom });
        }

        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int top1 = i * 2;
            unsigned int bottom1 = top1 + 1;
            unsigned int top2 = top1 + 2;
            unsigned int bottom2 = top1 + 3;

            indices.push_back(top1);
            indices.push_back(bottom1);
            indices.push_back(top2);

            indices.push_back(top2);
            indices.push_back(bottom1);
            indices.push_back(bottom2);
        }

        unsigned int centerTopIndex = static_cast<unsigned int>(vertices.size());
        vertices.push_back({ glm::vec3(0, height * 0.5f, 0), glm::vec3(0, 1, 0), glm::vec2(0.5f, 0.5f) });

		unsigned int centerBottomIndex = static_cast<unsigned int>(vertices.size());
        vertices.push_back({ glm::vec3(0, -height * 0.5f, 0), glm::vec3(0, -1, 0), glm::vec2(0.5f, 0.5f) });

        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int curr = i * 2;
            unsigned int next = (i < segments - 1) ? curr + 2 : 0;

            indices.push_back(centerTopIndex);
            indices.push_back(next);
            indices.push_back(curr);

            indices.push_back(centerBottomIndex);
            indices.push_back(curr + 1);
            indices.push_back(next + 1);
        }

        _cylinderMesh = std::make_unique<Mesh>();
        _cylinderMesh->LoadData(vertices, indices);
    }


    void CreateSphere(unsigned int latitudeSegments = 32, unsigned int longitudeSegments = 32, float radius = 1.0f) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int lat = 0; lat <= latitudeSegments; ++lat) {
            float theta = (float)lat / latitudeSegments * glm::pi<float>(); // 0 to PI
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (unsigned int lon = 0; lon <= longitudeSegments; ++lon) {
                float phi = (float)lon / longitudeSegments * glm::two_pi<float>(); // 0 to 2*PI
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                // Spherical to Cartesian coordinates
                float x = sinTheta * cosPhi;
                float y = cosTheta;
                float z = sinTheta * sinPhi;

                glm::vec3 position = glm::vec3(x * radius, y * radius, z * radius);
                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

                vertices.push_back({ position, normal });
            }
        }

        for (unsigned int lat = 0; lat < latitudeSegments; ++lat) {
            for (unsigned int lon = 0; lon < longitudeSegments; ++lon) {
                unsigned int first = lat * (longitudeSegments + 1) + lon;
                unsigned int second = first + longitudeSegments + 1;

                // First triangle
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                // Second triangle
                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        _sphereMesh = std::make_unique<Mesh>();
        _sphereMesh->LoadData(vertices, indices);
    }

    void CreateCapsule(unsigned int segments = 16, unsigned int rings = 8, float height = 2.0f, float radius = 0.5f) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float cylinderHeight = height - 2.0f * radius; 
        float halfCylinderHeight = cylinderHeight * 0.5f;

        for (unsigned int i = 0; i <= segments; ++i) {
            float theta = (float)i / segments * glm::two_pi<float>();
            float x = cos(theta);
            float z = sin(theta);

            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
            glm::vec3 posTop = glm::vec3(x * radius, halfCylinderHeight, z * radius);
            glm::vec3 posBottom = glm::vec3(x * radius, -halfCylinderHeight, z * radius);

            vertices.push_back({ posTop, normal });
            vertices.push_back({ posBottom, normal });
        }

        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int top1 = i * 2;
            unsigned int bottom1 = top1 + 1;
            unsigned int top2 = top1 + 2;
            unsigned int bottom2 = top1 + 3;

            // First triangle
            indices.push_back(top1);
            indices.push_back(bottom1);
            indices.push_back(top2);

            // Second triangle
            indices.push_back(top2);
            indices.push_back(bottom1);
            indices.push_back(bottom2);
        }

		unsigned int topHemisphereStart = static_cast<unsigned int>(vertices.size());
        for (unsigned int ring = 0; ring <= rings; ++ring) {
            float phi = (float)ring / rings * (glm::pi<float>() * 0.5f); // 0 to PI/2
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            float y = halfCylinderHeight + radius * cosPhi;

            for (unsigned int seg = 0; seg <= segments; ++seg) {
                float theta = (float)seg / segments * glm::two_pi<float>();
                float x = sinPhi * cos(theta);
                float z = sinPhi * sin(theta);

                glm::vec3 position = glm::vec3(x * radius, y, z * radius);
                glm::vec3 normal = glm::normalize(glm::vec3(x, cosPhi, z));

                vertices.push_back({ position, normal });
            }
        }

		unsigned int bottomHemisphereStart = static_cast<unsigned int>(vertices.size());
        for (unsigned int ring = 0; ring <= rings; ++ring) {
            float phi = (float)ring / rings * (glm::pi<float>() * 0.5f); // 0 to PI/2
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            float y = -halfCylinderHeight - radius * cosPhi;

            for (unsigned int seg = 0; seg <= segments; ++seg) {
                float theta = (float)seg / segments * glm::two_pi<float>();
                float x = sinPhi * cos(theta);
                float z = sinPhi * sin(theta);

                glm::vec3 position = glm::vec3(x * radius, y, z * radius);
                glm::vec3 normal = glm::normalize(glm::vec3(x, -cosPhi, z));

                vertices.push_back({ position, normal });
            }
        }

        for (unsigned int ring = 0; ring < rings; ++ring) {
            for (unsigned int seg = 0; seg < segments; ++seg) {
                unsigned int curr = topHemisphereStart + ring * (segments + 1) + seg;
                unsigned int next = curr + segments + 1;

                // First triangle
                indices.push_back(curr);
                indices.push_back(next);
                indices.push_back(curr + 1);

                // Second triangle
                indices.push_back(next);
                indices.push_back(next + 1);
                indices.push_back(curr + 1);
            }
        }

        for (unsigned int ring = 0; ring < rings; ++ring) {
            for (unsigned int seg = 0; seg < segments; ++seg) {
                unsigned int curr = bottomHemisphereStart + ring * (segments + 1) + seg;
                unsigned int next = curr + segments + 1;

                // First triangle (reversed winding for bottom)
                indices.push_back(curr);
                indices.push_back(curr + 1);
                indices.push_back(next);

                // Second triangle (reversed winding for bottom)
                indices.push_back(next);
                indices.push_back(curr + 1);
                indices.push_back(next + 1);
            }
        }

        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int cylinderTop = i * 2;
            unsigned int cylinderTopNext = cylinderTop + 2;
            unsigned int hemisphereBase = topHemisphereStart + i;
            unsigned int hemisphereBaseNext = topHemisphereStart + i + 1;

            // Triangle 1
            indices.push_back(cylinderTop);
            indices.push_back(hemisphereBase);
            indices.push_back(cylinderTopNext);

            // Triangle 2
            indices.push_back(cylinderTopNext);
            indices.push_back(hemisphereBase);
            indices.push_back(hemisphereBaseNext);
        }

        // Bottom connection
        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int cylinderBottom = i * 2 + 1;
            unsigned int cylinderBottomNext = cylinderBottom + 2;
            unsigned int hemisphereBase = bottomHemisphereStart + i;
            unsigned int hemisphereBaseNext = bottomHemisphereStart + i + 1;

            // Triangle 1 (reversed for bottom)
            indices.push_back(cylinderBottom);
            indices.push_back(cylinderBottomNext);
            indices.push_back(hemisphereBase);

            // Triangle 2 (reversed for bottom)
            indices.push_back(cylinderBottomNext);
            indices.push_back(hemisphereBaseNext);
            indices.push_back(hemisphereBase);
        }

        _capsuleMesh = std::make_unique<Mesh>();
        _capsuleMesh->LoadData(vertices, indices);
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
		CreateCylinder();
		CreateSphere();
		CreateCapsule();

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

        const glm::vec3 cubeColor(0.8f, 0.3f, 0.2f);
        for (const auto& pos : cubePositions) {
            Transform cube{
			    .position = pos,
			    .scale = glm::vec3(2.0f)
            };
            _solidColorShader->SetMat4("model", cube.ToMatrix());
            _solidColorShader->SetVec3("color", cubeColor);
            _cubeMesh->Draw();
        }

		// Soon To Be Playermodel For Testing
		const glm::vec3 capsuleColor(0.2f, 0.3f, 0.8f);
        Transform capsule{
            .position = glm::vec3(0.0f, 1.0f, -2.0f),
            .scale = glm::vec3(1.0f),
        };
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

    void ToggleWireframe() { _wireframeMode = !_wireframeMode; }
    void ToggleDebugInfo() { _showDebugInfo = !_showDebugInfo; }
}
