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

#include "../../../common.h"
#include "GL_mesh.h"

Mesh::~Mesh() {
    if (_VAO) glDeleteVertexArrays(1, &_VAO);
    if (_VBO) glDeleteBuffers(1, &_VBO);
    if (_EBO) glDeleteBuffers(1, &_EBO);
}

void Mesh::LoadData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    _indexCount = static_cast<GLuint>(indices.size());

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    if (_VAO && _indexCount > 0) {
        glBindVertexArray(_VAO);
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

bool Mesh::IsValid() const {
    return _VAO != 0 && _VBO != 0 && _EBO != 0 && _indexCount > 0;
}

namespace StaticMeshes
{
    std::unique_ptr<Mesh> GetQuad() {
        std::vector<Vertex> vertices = {
             {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
             {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
             {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
             {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
        };
        std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

        auto quadMesh = std::make_unique<Mesh>();
        quadMesh->LoadData(vertices, indices);

        return quadMesh;
    }

    std::unique_ptr<Mesh> StaticMeshes::GetCube() {
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

        auto cubeMesh = std::make_unique<Mesh>();
        cubeMesh->LoadData(cubeVertices, cubeIndices);

        return cubeMesh;
    }

    std::unique_ptr<Mesh> GetCylinder(unsigned int segments = 16, float height = 1.0f, float radius = 0.5f) {
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

        auto cylinderMesh = std::make_unique<Mesh>();
        cylinderMesh->LoadData(vertices, indices);

        return cylinderMesh;
    }

    std::unique_ptr<Mesh> GetSphere(unsigned int latitudeSegments = 16, unsigned int longitudeSegments = 16, float radius = 0.5f) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        for (unsigned int lat = 0; lat <= latitudeSegments; ++lat) {
            float theta = (float)lat / latitudeSegments * glm::pi<float>();
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);
            for (unsigned int lon = 0; lon <= longitudeSegments; ++lon) {
                float phi = (float)lon / longitudeSegments * glm::two_pi<float>();
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);
                glm::vec3 position = glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta) * radius;
                glm::vec3 normal = glm::normalize(position);
                glm::vec2 uv = glm::vec2((float)lon / longitudeSegments, (float)lat / latitudeSegments);
                vertices.push_back({ position, normal, uv });
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

        auto sphereMesh = std::make_unique<Mesh>();
        sphereMesh->LoadData(vertices, indices);

        return sphereMesh;
    }

    std::unique_ptr<Mesh> GetCapsule(unsigned int segments = 16, unsigned int rings = 8, float height = 2.0f, float radius = 0.5f) {
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

        auto capsuleMesh = std::make_unique<Mesh>();
        capsuleMesh->LoadData(vertices, indices);

        return capsuleMesh;
    }
}
