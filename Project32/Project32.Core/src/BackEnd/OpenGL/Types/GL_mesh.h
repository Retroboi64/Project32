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

#ifndef GL_MESH_H
#define GL_MESH_H

#include "../../../common.h"
#include "../../../renderer/vertex.h"
#include "GL_textures.h"

struct Vertex;

class Mesh {
public:
    struct Stats {
        size_t vertexCount = 0;
        size_t indexCount = 0;
        size_t triangleCount = 0;
        size_t memoryUsage = 0;  // In bytes
    };

    struct Bounds {
        glm::vec3 min{ 0.0f };
        glm::vec3 max{ 0.0f };
        glm::vec3 center{ 0.0f };
        float radius = 0.0f; 
    };

private:
    GLuint _VAO = 0;
    GLuint _VBO = 0;
    GLuint _EBO = 0;
    GLuint _indexCount = 0;
    GLuint _vertexCount = 0;
    std::string _name;
    bool m_isLoaded = false;
    Bounds m_bounds;

public:
    Mesh() = default;
    explicit Mesh(std::string name) : _name(std::move(name)) {}

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void LoadData(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    void LoadData(const std::vector<Vertex>& vertices);

    void SetName(std::string name) { _name = std::move(name); }

    [[nodiscard]] const std::string& GetName() const { return _name; }
    [[nodiscard]] GLuint GetIndexCount() const { return _indexCount; }
    [[nodiscard]] GLuint GetVertexCount() const { return _vertexCount; }
    [[nodiscard]] GLuint GetVAO() const { return _VAO; }
    [[nodiscard]] GLuint GetVBO() const { return _VBO; }
    [[nodiscard]] GLuint GetEBO() const { return _EBO; }
    [[nodiscard]] bool IsValid() const { return _VAO != 0 && m_isLoaded; }
    [[nodiscard]] bool IsIndexed() const { return _indexCount > 0; }
    [[nodiscard]] const Bounds& GetBounds() const { return m_bounds; }

    [[nodiscard]] Stats GetStats() const;

    void Draw() const;
    void DrawInstanced(unsigned int instanceCount) const;

    void Cleanup();

private:
    void CalculateBounds(const std::vector<Vertex>& vertices);
    void SetupMesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);
    void SetupMesh(const std::vector<Vertex>& vertices);
};

class MeshCache {
private:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshes;
    mutable std::mutex m_mutex;

public:
    MeshCache() = default;
    ~MeshCache() = default;

    MeshCache(const MeshCache&) = delete;
    MeshCache& operator=(const MeshCache&) = delete;
    MeshCache(MeshCache&&) noexcept = default;
    MeshCache& operator=(MeshCache&&) noexcept = default;

    template<typename Factory>
    Mesh* GetOrCreate(const std::string& key, Factory&& factory) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_meshes.find(key);
        if (it != m_meshes.end()) {
            return it->second.get();
        }

        auto mesh = factory();
        if (!mesh) {
            spdlog::error("[MeshCache] Factory returned null mesh for key: {}", key);
            return nullptr;
        }

        mesh->SetName(key);
        auto* ptr = mesh.get();
        m_meshes.emplace(key, std::move(mesh));

        spdlog::debug("[MeshCache] Created and cached mesh: {}", key);
        return ptr;
    }

    [[nodiscard]] Mesh* Get(const std::string& key);
    [[nodiscard]] bool Has(const std::string& key) const;

    bool Add(const std::string& key, std::unique_ptr<Mesh> mesh);
    bool Remove(const std::string& key);

    void Clear();
    [[nodiscard]] size_t Size() const;
    [[nodiscard]] std::vector<std::string> GetKeys() const;
    [[nodiscard]] size_t GetTotalMemoryUsage() const;
};

// TODO: Do this later
//class MeshManager {
//private:
//	int m_nextMeshID = 0;
//	std::vector<std::unique_ptr<Mesh>> m_meshes;
//
//public:
//    MeshManager() = default;
//    ~MeshManager() = default;
//
//    void AddMesh(std::unique_ptr<Mesh> mesh) {
//		m_meshes.push_back(std::move(mesh));
//	}
//
//    void RemoveMesh(size_t index) {
//        if (index < m_meshes.size()) {
//            m_meshes.erase(m_meshes.begin() + index);
//        }
//    }
//
//    Mesh* GetMesh(size_t index) const {
//        if (index >= m_meshes.size()) {
//            return nullptr;
//        }
//        return m_meshes[index].get();
//	}
//
//    Mesh* GetMeshByName(const std::string& name) const {
//        for (const auto& mesh : m_meshes) {
//            if (mesh->GetName() == name) {
//                return mesh.get();
//            }
//        }
//        return nullptr;
//	}
//};

namespace StaticMeshes {
    struct CylinderParams {
        unsigned int segments = 16;
        float height = 1.0f;
        float radius = 0.5f;
    };

    struct SphereParams {
        unsigned int latitudeSegments = 16;
        unsigned int longitudeSegments = 16;
        float radius = 0.5f;
    };

    struct CapsuleParams {
        unsigned int segments = 16;
        unsigned int rings = 8;
        float height = 2.0f;
        float radius = 0.5f;
    };

    struct ConeParams {
        unsigned int segments = 16;
        float height = 1.0f;
        float radius = 0.5f;
    };

    struct TorusParams {
        unsigned int majorSegments = 24;
        unsigned int minorSegments = 16;
        float majorRadius = 1.0f;
        float minorRadius = 0.3f;
    };

    std::unique_ptr<Mesh> GetQuad();
    std::unique_ptr<Mesh> GetCube();
    std::unique_ptr<Mesh> GetPlane(float width = 1.0f, float height = 1.0f,
        unsigned int subdivisions = 1);

    std::unique_ptr<Mesh> GetCylinder(const CylinderParams& params = {});
    std::unique_ptr<Mesh> GetSphere(const SphereParams& params = {});
    std::unique_ptr<Mesh> GetCapsule(const CapsuleParams& params = {});
    std::unique_ptr<Mesh> GetCone(const ConeParams& params = {});
    std::unique_ptr<Mesh> GetTorus(const TorusParams& params = {});

    inline std::unique_ptr<Mesh> GetCylinder(unsigned int segments, float height, float radius) {
        return GetCylinder({ segments, height, radius });
    }

    inline std::unique_ptr<Mesh> GetSphere(unsigned int latitudeSegments,
        unsigned int longitudeSegments,
        float radius) {
        return GetSphere({ latitudeSegments, longitudeSegments, radius });
    }

    inline std::unique_ptr<Mesh> GetCapsule(unsigned int segments, unsigned int rings,
        float height, float radius) {
        return GetCapsule({ segments, rings, height, radius });
    }
}

#endif // GL_MESH_H