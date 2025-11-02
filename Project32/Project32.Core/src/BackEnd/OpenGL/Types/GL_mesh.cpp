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
#include "../../../renderer/vertex.h"

Mesh::~Mesh() {
    Cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : _VAO(other._VAO)
    , _VBO(other._VBO)
    , _EBO(other._EBO)
    , _indexCount(other._indexCount)
    , _vertexCount(other._vertexCount)
    , _name(std::move(other._name))
    , m_isLoaded(other.m_isLoaded)
    , m_bounds(other.m_bounds)
{
    other._VAO = 0;
    other._VBO = 0;
    other._EBO = 0;
    other._indexCount = 0;
    other._vertexCount = 0;
    other.m_isLoaded = false;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Cleanup();

        _VAO = other._VAO;
        _VBO = other._VBO;
        _EBO = other._EBO;
        _indexCount = other._indexCount;
        _vertexCount = other._vertexCount;
        _name = std::move(other._name);
        m_isLoaded = other.m_isLoaded;
        m_bounds = other.m_bounds;

        other._VAO = 0;
        other._VBO = 0;
        other._EBO = 0;
        other._indexCount = 0;
        other._vertexCount = 0;
        other.m_isLoaded = false;
    }
    return *this;
}

void Mesh::LoadData(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {
    if (vertices.empty()) {
        spdlog::warn("[Mesh::LoadData] Attempted to load empty vertex data for mesh: {}", _name);
        return;
    }

    if (indices.empty()) {
        spdlog::warn("[Mesh::LoadData] Empty indices, falling back to non-indexed rendering");
        LoadData(vertices);
        return;
    }

    CalculateBounds(vertices);
    SetupMesh(vertices, indices);

    _indexCount = static_cast<GLuint>(indices.size());
    _vertexCount = static_cast<GLuint>(vertices.size());
    m_isLoaded = true;

    spdlog::debug("[Mesh] Loaded indexed mesh '{}': {} vertices, {} indices",
        _name, _vertexCount, _indexCount);
}

void Mesh::LoadData(const std::vector<Vertex>& vertices) {
    if (vertices.empty()) {
        spdlog::warn("[Mesh::LoadData] Attempted to load empty vertex data for mesh: {}", _name);
        return;
    }

    CalculateBounds(vertices);
    SetupMesh(vertices);

    _vertexCount = static_cast<GLuint>(vertices.size());
    _indexCount = 0;
    m_isLoaded = true;

    spdlog::debug("[Mesh] Loaded non-indexed mesh '{}': {} vertices", _name, _vertexCount);
}

void Mesh::Draw() const {
    if (!IsValid()) {
        spdlog::warn("[Mesh::Draw] Attempted to draw invalid mesh: {}", _name);
        return;
    }

    glBindVertexArray(_VAO);

    if (IsIndexed()) {
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
    }

    glBindVertexArray(0);
}

void Mesh::DrawInstanced(unsigned int instanceCount) const {
    if (!IsValid()) {
        spdlog::warn("[Mesh::DrawInstanced] Attempted to draw invalid mesh: {}", _name);
        return;
    }

    if (instanceCount == 0) {
        spdlog::warn("[Mesh::DrawInstanced] Instance count is 0");
        return;
    }

    glBindVertexArray(_VAO);

    if (IsIndexed()) {
        glDrawElementsInstanced(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
    }
    else {
        glDrawArraysInstanced(GL_TRIANGLES, 0, _vertexCount, instanceCount);
    }

    glBindVertexArray(0);
}

void Mesh::Cleanup() {
    if (_VAO != 0) {
        glDeleteVertexArrays(1, &_VAO);
        _VAO = 0;
    }

    if (_VBO != 0) {
        glDeleteBuffers(1, &_VBO);
        _VBO = 0;
    }

    if (_EBO != 0) {
        glDeleteBuffers(1, &_EBO);
        _EBO = 0;
    }

    _indexCount = 0;
    _vertexCount = 0;
    m_isLoaded = false;
}

Mesh::Stats Mesh::GetStats() const {
    Stats stats;
    stats.vertexCount = _vertexCount;
    stats.indexCount = _indexCount;
    stats.triangleCount = IsIndexed() ? (_indexCount / 3) : (_vertexCount / 3);

    stats.memoryUsage = _vertexCount * sizeof(Vertex);
    if (IsIndexed()) {
        stats.memoryUsage += _indexCount * sizeof(unsigned int);
    }

    return stats;
}

void Mesh::CalculateBounds(const std::vector<Vertex>& vertices) {
    if (vertices.empty()) {
        m_bounds = Bounds{};
        return;
    }

    glm::vec3 min = vertices[0].position;
    glm::vec3 max = vertices[0].position;

    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex.position);
        max = glm::max(max, vertex.position);
    }

    m_bounds.min = min;
    m_bounds.max = max;
    m_bounds.center = (min + max) * 0.5f;
    m_bounds.radius = glm::length(max - m_bounds.center);
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices) {
    Cleanup();

    // Generate buffers
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
        vertices.data(), GL_STATIC_DRAW);

    // Load index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, position));

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, normal));

    // TexCoords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, texCoord));

    // Color attribute (if exists in Vertex struct)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices) {
    Cleanup();

    // Generate buffers
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);

    glBindVertexArray(_VAO);

    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
        vertices.data(), GL_STATIC_DRAW);

    // Attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

// MeshCache Implementation
Mesh* MeshCache::Get(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_meshes.find(key);
    return (it != m_meshes.end()) ? it->second.get() : nullptr;
}

bool MeshCache::Has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_meshes.find(key) != m_meshes.end();
}

bool MeshCache::Add(const std::string& key, std::unique_ptr<Mesh> mesh) {
    if (!mesh) {
        spdlog::warn("[MeshCache::Add] Attempted to add null mesh with key: {}", key);
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_meshes.find(key) != m_meshes.end()) {
        spdlog::warn("[MeshCache::Add] Mesh with key '{}' already exists", key);
        return false;
    }

    mesh->SetName(key);
    m_meshes.emplace(key, std::move(mesh));
    spdlog::debug("[MeshCache] Added mesh: {}", key);

    return true;
}

bool MeshCache::Remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_meshes.find(key);
    if (it != m_meshes.end()) {
        spdlog::debug("[MeshCache] Removed mesh: {}", key);
        m_meshes.erase(it);
        return true;
    }

    return false;
}

void MeshCache::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t count = m_meshes.size();
    m_meshes.clear();
    spdlog::info("[MeshCache] Cleared {} cached meshes", count);
}

size_t MeshCache::Size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_meshes.size();
}

std::vector<std::string> MeshCache::GetKeys() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> keys;
    keys.reserve(m_meshes.size());

    for (const auto& [key, _] : m_meshes) {
        keys.push_back(key);
    }

    return keys;
}

size_t MeshCache::GetTotalMemoryUsage() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t total = 0;

    for (const auto& [_, mesh] : m_meshes) {
        if (mesh) {
            total += mesh->GetStats().memoryUsage;
        }
    }

    return total;
}

// Static Meshes
namespace StaticMeshes {

    std::unique_ptr<Mesh> GetQuad() {
        std::vector<Vertex> vertices = {
            // Position              Normal              TexCoords      Color
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };

        auto mesh = std::make_unique<Mesh>("Quad");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetCube() {
        std::vector<Vertex> vertices = {
            // Front face
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},

            // Back face
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},

            // Top face
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},

            // Bottom face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},

            // Right face
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},

            // Left face
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}
        };

        std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0,       // Front
            4, 5, 6, 6, 7, 4,       // Back
            8, 9, 10, 10, 11, 8,    // Top
            12, 13, 14, 14, 15, 12, // Bottom
            16, 17, 18, 18, 19, 16, // Right
            20, 21, 22, 22, 23, 20  // Left
        };

        auto mesh = std::make_unique<Mesh>("Cube");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetPlane(float width, float height, unsigned int subdivisions) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        subdivisions = std::max(1u, subdivisions);

        float stepX = width / static_cast<float>(subdivisions);
        float stepZ = height / static_cast<float>(subdivisions);

        // Generate vertices
        for (unsigned int z = 0; z <= subdivisions; ++z) {
            for (unsigned int x = 0; x <= subdivisions; ++x) {
                float xPos = -width / 2.0f + x * stepX;
                float zPos = -height / 2.0f + z * stepZ;

                Vertex vertex{
                    glm::vec3(xPos, 0.0f, zPos),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec2(
                        static_cast<float>(x) / subdivisions,
                        static_cast<float>(z) / subdivisions
                    ),
                    glm::vec3(1.0f),
                };
                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (unsigned int z = 0; z < subdivisions; ++z) {
            for (unsigned int x = 0; x < subdivisions; ++x) {
                unsigned int topLeft = z * (subdivisions + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (z + 1) * (subdivisions + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        auto mesh = std::make_unique<Mesh>("Plane");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetCylinder(const CylinderParams& params) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        const float angleStep = 2.0f * glm::pi<float>() / params.segments;
        const float halfHeight = params.height / 2.0f;

        // Generate side vertices
        for (unsigned int i = 0; i <= params.segments; ++i) {
            float angle = i * angleStep;
            float x = params.radius * cos(angle);
            float z = params.radius * sin(angle);
            float u = static_cast<float>(i) / params.segments;

            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

            // Top vertex
            Vertex topVertex{
                glm::vec3(x, halfHeight, z),
                normal,
                glm::vec2(u, 1.0f),
                glm::vec3(1.0f)
            };
            vertices.push_back(topVertex);

            // Bottom vertex
            Vertex bottomVertex{
                glm::vec3(x, -halfHeight, z),
                normal,
                glm::vec2(u, 0.0f),
                glm::vec3(1.0f)
            };
            vertices.push_back(bottomVertex);
        }

        // Generate side indices
        for (unsigned int i = 0; i < params.segments; ++i) {
            unsigned int topLeft = i * 2;
            unsigned int bottomLeft = topLeft + 1;
            unsigned int topRight = (i + 1) * 2;
            unsigned int bottomRight = topRight + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }

        // Add caps (top and bottom)
        unsigned int centerTopIndex = static_cast<unsigned int>(vertices.size());
        Vertex centerTop{
            glm::vec3(0.0f, halfHeight, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.5f, 0.5f),
		    glm::vec3(1.0f) 
        };
        vertices.push_back(centerTop);

        unsigned int centerBottomIndex = static_cast<unsigned int>(vertices.size());
        Vertex centerBottom{
            glm::vec3(0.0f, -halfHeight, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec2(0.5f, 0.5f),
            glm::vec3(1.0f)
        };
        vertices.push_back(centerBottom);

        // Cap vertices and indices
        for (unsigned int i = 0; i < params.segments; ++i) {
            float angle = i * angleStep;
            float x = params.radius * cos(angle);
            float z = params.radius * sin(angle);

            // Top cap vertex
            Vertex topCapVertex{
                glm::vec3(x, halfHeight, z),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle)),
                glm::vec3(1.0f)
            };
            vertices.push_back(topCapVertex);

            // Bottom cap vertex
            Vertex bottomCapVertex{
                glm::vec3(x, -halfHeight, z),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec2(0.5f + 0.5f * cos(angle), 0.5f - 0.5f * sin(angle)),
                glm::vec3(1.0f),
            };
            vertices.push_back(bottomCapVertex);
        }

        unsigned int capStartIndex = centerBottomIndex + 1;
        for (unsigned int i = 0; i < params.segments; ++i) {
            unsigned int current = capStartIndex + i * 2;
            unsigned int next = capStartIndex + ((i + 1) % params.segments) * 2;

            // Top cap
            indices.push_back(centerTopIndex);
            indices.push_back(current);
            indices.push_back(next);

            // Bottom cap
            indices.push_back(centerBottomIndex);
            indices.push_back(next + 1);
            indices.push_back(current + 1);
        }

        auto mesh = std::make_unique<Mesh>("Cylinder");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetSphere(const SphereParams& params) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Generate vertices
        for (unsigned int lat = 0; lat <= params.latitudeSegments; ++lat) {
            float theta = lat * glm::pi<float>() / params.latitudeSegments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (unsigned int lon = 0; lon <= params.longitudeSegments; ++lon) {
                float phi = lon * 2.0f * glm::pi<float>() / params.longitudeSegments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                Vertex vertex{
					glm::vec3(
                        params.radius * sinTheta * cosPhi,
                        params.radius * cosTheta,
                        params.radius * sinTheta * sinPhi
					),

                    glm::normalize(vertex.position),

                    glm::vec2(
                        static_cast<float>(lon) / params.longitudeSegments,
                        static_cast<float>(lat) / params.latitudeSegments
                    ),

                    //1.0f - (static_cast<float>(lon) / params.longitudeSegments),
                    //1.0f - (static_cast<float>(lat) / params.latitudeSegments),

                    glm::vec3(1.0f)
                };


                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (unsigned int lat = 0; lat < params.latitudeSegments; ++lat) {
            for (unsigned int lon = 0; lon < params.longitudeSegments; ++lon) {
                unsigned int first = lat * (params.longitudeSegments + 1) + lon;
                unsigned int second = first + params.longitudeSegments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        auto mesh = std::make_unique<Mesh>("Sphere");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetCapsule(const CapsuleParams& params) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        const float cylinderHeight = params.height - 2.0f * params.radius;
        const float halfCylinderHeight = cylinderHeight / 2.0f;
        const float angleStep = 2.0f * glm::pi<float>() / params.segments;

        // Generate cylinder body
        for (unsigned int i = 0; i <= params.segments; ++i) {
            float angle = i * angleStep;
            float x = params.radius * cos(angle);
            float z = params.radius * sin(angle);
            float u = static_cast<float>(i) / params.segments;

            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

            // Top vertex
            Vertex topVertex{
                glm::vec3(x, halfCylinderHeight, z),
                normal,
                glm::vec2(u, 0.75f),
                glm::vec3(1.0f)
            };
            vertices.push_back(topVertex);

            // Bottom vertex
            Vertex bottomVertex{
                glm::vec3(x, -halfCylinderHeight, z),
                normal,
                glm::vec2(u, 0.25f),
                glm::vec3(1.0f)
            };

            vertices.push_back(bottomVertex);
        }

        // Generate cylinder indices
        for (unsigned int i = 0; i < params.segments; ++i) {
            unsigned int topLeft = i * 2;
            unsigned int bottomLeft = topLeft + 1;
            unsigned int topRight = (i + 1) * 2;
            unsigned int bottomRight = topRight + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }

        // Generate top hemisphere
        unsigned int hemisphereStartIndex = static_cast<unsigned int>(vertices.size());

        for (unsigned int ring = 0; ring <= params.rings; ++ring) {
            float phi = (glm::pi<float>() / 2.0f) * (static_cast<float>(ring) / params.rings);
            float y = params.radius * sin(phi);
            float ringRadius = params.radius * cos(phi);

            for (unsigned int seg = 0; seg <= params.segments; ++seg) {
                float theta = seg * angleStep;
                float x = ringRadius * cos(theta);
                float z = ringRadius * sin(theta);

                Vertex vertex{
                    glm::vec3(x, y + halfCylinderHeight, z),
                    glm::normalize(glm::vec3(x, y, z)),
                    glm::vec2(
                        static_cast<float>(seg) / params.segments,
                        0.75f + 0.25f * (static_cast<float>(ring) / params.rings)
                    ),
                    glm::vec3(1.0f)
                };
                vertices.push_back(vertex);
            }
        }

        // Generate top hemisphere indices
        for (unsigned int ring = 0; ring < params.rings; ++ring) {
            for (unsigned int seg = 0; seg < params.segments; ++seg) {
                unsigned int current = hemisphereStartIndex + ring * (params.segments + 1) + seg;
                unsigned int next = current + params.segments + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        // Generate bottom hemisphere
        hemisphereStartIndex = static_cast<unsigned int>(vertices.size());

        for (unsigned int ring = 0; ring <= params.rings; ++ring) {
            float phi = (glm::pi<float>() / 2.0f) * (static_cast<float>(ring) / params.rings);
            float y = -params.radius * sin(phi);
            float ringRadius = params.radius * cos(phi);

            for (unsigned int seg = 0; seg <= params.segments; ++seg) {
                float theta = seg * angleStep;
                float x = ringRadius * cos(theta);
                float z = ringRadius * sin(theta);

                Vertex vertex{
                    glm::vec3(x, y - halfCylinderHeight, z),
                    glm::normalize(glm::vec3(x, y, z)),
                    glm::vec2(
                        static_cast<float>(seg) / params.segments,
                        0.25f - 0.25f * (static_cast<float>(ring) / params.rings)
                    ),
                    glm::vec3(1.0f)
                };
                vertices.push_back(vertex);
            }
        }

        // Generate bottom hemisphere indices
        for (unsigned int ring = 0; ring < params.rings; ++ring) {
            for (unsigned int seg = 0; seg < params.segments; ++seg) {
                unsigned int current = hemisphereStartIndex + ring * (params.segments + 1) + seg;
                unsigned int next = current + params.segments + 1;

                indices.push_back(current);
                indices.push_back(current + 1);
                indices.push_back(next);

                indices.push_back(current + 1);
                indices.push_back(next + 1);
                indices.push_back(next);
            }
        }

        auto mesh = std::make_unique<Mesh>("Capsule");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetCone(const ConeParams& params) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        const float angleStep = 2.0f * glm::pi<float>() / params.segments;
        const float halfHeight = params.height / 2.0f;

        // Apex vertex
        Vertex apex{
            glm::vec3(0.0f, halfHeight, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.5f, 1.0f),
            glm::vec3(1.0f)
        };
        vertices.push_back(apex);

        // Generate side vertices
        for (unsigned int i = 0; i <= params.segments; ++i) {
            float angle = i * angleStep;
            float x = params.radius * cos(angle);
            float z = params.radius * sin(angle);

            // Calculate normal (pointing outward and slightly up)
            glm::vec3 toApex = glm::normalize(glm::vec3(0.0f, halfHeight, 0.0f) - glm::vec3(x, -halfHeight, z));
            glm::vec3 radial = glm::normalize(glm::vec3(x, 0.0f, z));
            glm::vec3 normal = glm::normalize(glm::cross(
                glm::cross(toApex, radial),
                toApex
            ));

            Vertex vertex{
                glm::vec3(x, -halfHeight, z),
                normal,
                glm::vec2(static_cast<float>(i) / params.segments, 0.0f),
                glm::vec3(1.0f)
            };
            vertices.push_back(vertex);
        }

        // Generate side indices
        for (unsigned int i = 0; i < params.segments; ++i) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
        }

        // Base center vertex
        unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size());
        Vertex baseCenter{
            glm::vec3(0.0f, -halfHeight, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec2(0.5f, 0.5f),
            glm::vec3(1.0f)
        };
        vertices.push_back(baseCenter);

        // Generate base vertices
        unsigned int baseStartIndex = static_cast<unsigned int>(vertices.size());
        for (unsigned int i = 0; i < params.segments; ++i) {
            float angle = i * angleStep;
            float x = params.radius * cos(angle);
            float z = params.radius * sin(angle);

            Vertex vertex{
                glm::vec3(x, -halfHeight, z),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec2(
                    0.5f + 0.5f * cos(angle),
                    0.5f + 0.5f * sin(angle)
                ),
                glm::vec3(1.0f)
            };

            vertices.push_back(vertex);
        }

        // Generate base indices
        for (unsigned int i = 0; i < params.segments; ++i) {
            unsigned int next = (i + 1) % params.segments;
            indices.push_back(baseCenterIndex);
            indices.push_back(baseStartIndex + next);
            indices.push_back(baseStartIndex + i);
        }

        auto mesh = std::make_unique<Mesh>("Cone");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

    std::unique_ptr<Mesh> GetTorus(const TorusParams& params) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        const float majorAngleStep = 2.0f * glm::pi<float>() / params.majorSegments;
        const float minorAngleStep = 2.0f * glm::pi<float>() / params.minorSegments;

        // Generate vertices
        for (unsigned int i = 0; i <= params.majorSegments; ++i) {
            float majorAngle = i * majorAngleStep;
            float cosMajor = cos(majorAngle);
            float sinMajor = sin(majorAngle);

            for (unsigned int j = 0; j <= params.minorSegments; ++j) {
                float minorAngle = j * minorAngleStep;
                float cosMinor = cos(minorAngle);
                float sinMinor = sin(minorAngle);

                float r = params.majorRadius + params.minorRadius * cosMinor;
                glm::vec3 center(params.majorRadius * cosMajor, 0.0f, params.majorRadius * sinMajor);

                Vertex vertex{
					// Position
                    glm::vec3((r * cosMajor),(params.minorRadius * sinMinor),(r * sinMajor)),

					// Normal
                    glm::normalize(vertex.position - center),

					// Texture coordinates
                    glm::vec2((static_cast<float>(i) / params.majorSegments),(static_cast<float>(j) / params.minorSegments)),

					// Color
                    glm::vec3(1.0f)
                };
                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (unsigned int i = 0; i < params.majorSegments; ++i) {
            for (unsigned int j = 0; j < params.minorSegments; ++j) {
                unsigned int first = i * (params.minorSegments + 1) + j;
                unsigned int second = first + params.minorSegments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        auto mesh = std::make_unique<Mesh>("Torus");
        mesh->LoadData(vertices, indices);
        return mesh;
    }

} // namespace StaticMeshes