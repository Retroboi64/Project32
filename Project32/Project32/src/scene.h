#pragma once

#include "common.h"
#include "mesh.h"
#include "model.h"
#include <memory>
#include <vector>
#include <string>

struct SceneMesh {
    std::unique_ptr<Mesh> mesh;
    std::string name;
    Transform transform{
        .position = glm::vec3(0.0f),
        .rotation = glm::vec3(0.0f),
		.scale = glm::vec3(1.0f)
    };
    bool isVisible = true;

    SceneMesh(std::unique_ptr<Mesh> m, const std::string& n)
        : mesh(std::move(m)), name(n) {
    }
};

class Scene {
private:
    std::vector<std::unique_ptr<SceneMesh>> _meshes;
    std::vector<std::unique_ptr<ModelImporter::LoadedModel>> _loadedModels;

public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void AddQuad(const std::string& name = "Quad");
    void AddCube(const std::string& name = "Cube");
    void AddCylinder(const std::string& name = "Cylinder", unsigned int segments = 16, float height = 1.0f, float radius = 0.5f);
    void AddSphere(const std::string& name = "Sphere", unsigned int latSegments = 16, unsigned int lonSegments = 16, float radius = 0.5f);
    void AddCapsule(const std::string& name = "Capsule", unsigned int segments = 16, unsigned int rings = 8, float height = 2.0f, float radius = 0.5f);

    bool LoadModel(const std::string& filePath, const std::string& namePrefix = "", bool generateNormals = true, bool flipTextureCoords = false);

    SceneMesh* GetMesh(const std::string& name);
    SceneMesh* GetMesh(size_t index);
    size_t GetMeshCount() const { return _meshes.size(); }

    void RemoveMesh(const std::string& name);
    void RemoveMesh(size_t index);
    void Clear();

    void DrawAll() const;
    void DrawMesh(const std::string& name) const;
    void DrawMesh(size_t index) const;

    void PrintMeshInfo() const;
    std::vector<std::string> GetMeshNames() const;
};

class SceneManager {
private:
    std::unique_ptr<Scene> _currentScene;

public:
    SceneManager();
    ~SceneManager() = default;

    void CreateNewScene();
    Scene* GetCurrentScene() { return _currentScene.get(); }

    void LoadSceneFromFile(const std::string& configFile);
};